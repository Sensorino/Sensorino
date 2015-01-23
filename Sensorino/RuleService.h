#include <Arduino.h>
#include <avr/eeprom.h>

#include "Service.h"
#include "Expression.h"

#define IS_TRUE(flt) (flt > 0.5f)
#define IS_ZERO(flt) ((flt) < EPSILON && -(flt) < EPSILON)

using namespace Data;

/* Use EEPROM for rule storage, rather than RAM */
#define EEPROM

class RuleService : public Service {
public:
    RuleService() : Service(1) {
        /* Check if rule storage is initialised and possilby reset it */
        validateStore();

        /* Mark all entries in variable cache as empty */
        for (uint8_t i = 0; i < ARRAY_SIZE(valueCache); i++)
            valueCache[i].serviceId = 0xff;
    }

    void evalPublish(Message &message) {
        int servId;
        uint32_t useMask = 0;

        if (!message.find(SERVICE_ID, 0, &servId))
            return;

        int offset = 0;

        /* Iterate over the ruleset, find rules that depend on the
         * variables from the service that emitted this message.
         */
        while (1) {
            uint8_t id = getByte(offset);
            if (id == 0xff)
                break;

            uint8_t conditionLen = getByte(offset + 1);
            uint8_t actionLen = getByte(offset + 2);
            int conditionOffset = offset + 3;
            int actionOffset = offset + 3 + conditionLen;
            float result;
            uint32_t ruleUseMask = 0;

            result = evalExpression(conditionOffset, servId, message,
                    ruleUseMask);

            /* Mark variables present in this rule as used */
            useMask |= ruleUseMask;

            if (ruleUseMask && !isnan(result) && IS_TRUE(result)) {
                /* Create a message with empty header and given payload */
                Message m;
                uint8_t *msgBuf = m.getWriteBuffer() + HEADERS_LENGTH;
                uint8_t i = actionLen;
                while (i--)
                    *msgBuf++ = getByte(actionOffset++);
                m.writeLength(HEADERS_LENGTH + actionLen);

                m.setSrcAddress(sensorino->getAddress());
                m.setDstAddress(sensorino->getAddress());
                m.setType(Message::SET);

                /* Execute the action */
                sensorino->handleMessage(m);
            }

            offset += 3 + conditionLen + actionLen;
        }

        CachedValue *v = valueCache;
        while (useMask) {
            if (useMask & 1) {
                uint32_t value;
                message.find(v->type, v->num, &value);
                v->value = Message::toFloat(v->type, &value);
            }
            useMask >>= 1;
            v++;
        }
    }

protected:
    struct CachedValue {
        Type type;
        uint8_t serviceId;
        uint8_t num;
        float value;
    } valueCache[10];

#ifndef EEPROM
#define RULE_STORE_SIZE 128
    uint8_t buffer[RULE_STORE_SIZE];

    uint8_t getByte(int offset) {
        return buffer[offset];
    }
    void setByte(int offset, uint8_t val) {
        buffer[offset] = val;
    }
    void validateStore(void) {
        /* Mark rule storage as empty to initialise it */
        setByte(0, 0xff);
    }
#else
#define BASE_ADDR 64
#define RULE_STORE_SIZE (511 - BASE_ADDR)

    uint8_t getByte(int addr) {
        while (!eeprom_is_ready());

        EEAR = BASE_ADDR + addr;
        EECR |= 1 << EERE;	/* Start eeprom read by writing EERE */

        return EEDR;
    }

    void setByte(int addr, uint8_t val) {
        while (!eeprom_is_ready());

        EEAR = BASE_ADDR + addr;
        EEDR = val;
        EECR |= 1 << EEMPE;	/* Write logical one to EEMPE */
        EECR |= 1 << EEPE;	/* Start eeprom write by setting EEPE */
    }

    void validateStore(void) {
        /* Check if magic value present after the end of our EEPROM space */
        if (getByte(RULE_STORE_SIZE) == 0xab)
            return;

        /* Mark rule storage as empty and write magic value */
        setByte(0, 0xff);
        setByte(RULE_STORE_SIZE, 0xab);
    }
#endif

    void onSet(Message *message) {
        int ruleId, offset;
        Message::BinaryValue condition, action;

        if (!message->find(COUNT, 0, &ruleId)) {
            err(message, COUNT)->send();
            return;
        }

        offset = findRule(ruleId);

        if (!message->find(EXPRESSION, 0, NULL) &&
                !message->find(MESSAGE, 0, NULL)) {
            if (offset < 0)
                err(message, COUNT)->send();

            /* Delete this rule and everything after it... */
            setByte(offset, 0xff);
            return;
        }

        /* Create a new rule or update an existing one */
        if (!message->find(EXPRESSION, 0, &condition) ||
                !message->find(MESSAGE, 0, &action)) {
            if (offset < 0)
                err(message, COUNT)->send();

            /* TODO: updating not supported */
            err(message)->send();
            return;
        }

        /* Create a new rule */
        if (offset >= 0) {
            err(message, COUNT)->send();
            return;
        }

        createRule(ruleId, condition, action);
    }

    void onRequest(Message *message) {
        int ruleId, offset;
        uint8_t num, typeExpr, typeMsg, typeType;
        Type dt;

        /* See what data types are being requested */
        while (message->find(DATATYPE, num++, &dt))
            if (dt == DATATYPE)
                typeType = 1;
            else if (dt == EXPRESSION)
                typeExpr = 1;
            else if (dt == MESSAGE)
                typeMsg = 1;
            else {
                err(message, dt)->send();
                return;
            }

        if (!message->find(COUNT, 0, &ruleId)) {
            if (typeType) {
                /* Send service description */
                Message *m = publish(message);
                m->addIntValue(COUNT, 0);
                m->addIntValue(COUNT, 2);
                m->addDataTypeValue(EXPRESSION);
                m->addDataTypeValue(MESSAGE);
                m->send();
            } else
                err(message, COUNT)->send();
            return;
        }

        offset = findRule(ruleId);

        if (offset < 0) {
            err(message, COUNT)->send();
            return;
        }

        uint8_t conditionLen = getByte(offset + 1);
        uint8_t actionLen = getByte(offset + 2);
        int conditionOffset = offset + 3;
        int actionOffset = offset + 3 + conditionLen;
        uint8_t buf[100];

        Message *resp = publish(message);

        resp->addIntValue(COUNT, ruleId);

        if (typeExpr) {
            for (uint8_t i = 0; i < conditionLen; i++)
                buf[i] = getByte(conditionOffset + i);
            resp->addBinaryValue(EXPRESSION, buf, conditionLen);
        }

        if (typeMsg) {
            for (uint8_t i = 0; i < actionLen; i++)
                buf[i] = getByte(actionOffset + i);
            resp->addBinaryValue(MESSAGE, buf, actionLen);
        }

        resp->send();
    }

    void createRule(uint8_t ruleId,
            Message::BinaryValue &condition, Message::BinaryValue &action) {
        int offset;

        /* Find empty space in the buffer */
        offset = findRule(0xff);

        if (offset + 4 + condition.len + action.len > RULE_STORE_SIZE) {
            err()->send();
            return;
        }

        setByte(offset++, ruleId);
        setByte(offset++, condition.len);
        setByte(offset++, action.len);

        for (uint8_t i = 0; i < condition.len; i++)
            setByte(offset++, condition.value[i]);

        for (uint8_t i = 0; i < action.len; i++)
            setByte(offset++, action.value[i]);

        setByte(offset, 0xff);
    }

    int findRule(uint8_t ruleId) {
        int offset = 0;

        while (1) {
            uint8_t id = getByte(offset);
            if (id == ruleId) {
                return offset;
            }
            if (id == 0xff)
                /* We've found empty space */
                break;

            uint8_t conditionLen = getByte(offset + 1);
            uint8_t actionLen = getByte(offset + 2);
            offset += 3 + conditionLen + actionLen;
        }

        return -1;
    }

    /** Evaluate the expression and return its current value.  Evaluation
     * is very simple and sort of like JavaScript in that there are no
     * type errors and everything is cast to floats as a most general type.
     * Precision problems may cause strange issues.
     *
     * TODO: size limit
     *
     * @param expr offset into the rule buffer where the expression starts,
     * gets updated to point at the end of the expression before this
     * method returns.
     * @param servId ID of the service that published @m.
     * @param m message in which to look for new values of referenced
     * variables.
     * @param useMask bitmask with one bit per variable cache entry.
     * This method sets the bits corresponding to variables referenced
     * by this expression and whose new value is available in @m.
     * @return expression's current value.
     */
    float evalExpression(int &expr, uint8_t servId, Message &m,
            uint32_t &useMask) {
        uint8_t op = getByte(expr++);

        float op1, op2, diff, ret;

        uint8_t varServId, varNum, i, b;
        Type varType;
        int16_t intVal;

        using namespace Expression;

        switch (op) {
        case VAL_INT8:
            return (int8_t) getByte(expr++);

        case VAL_INT16:
            intVal = ((uint16_t) getByte(expr) << 8) | getByte(expr + 1);
            expr += 2;
            return intVal;

        case VAL_FLOAT:
            *(uint32_t *) &ret =
                ((uint32_t) getByte(expr + 0) << 24) |
                ((uint32_t) getByte(expr + 1) << 16) |
                ((uint32_t) getByte(expr + 2) <<  8) |
                ((uint32_t) getByte(expr + 3) <<  0);
            expr += 4;
            return ret;

        case VAL_VARIABLE:
        case VAL_PREVIOUS:
            varServId = getByte(expr++);
            varType = (Type) getByte(expr++);
            varNum = getByte(expr++);
            return getVariableValue(m, servId, varServId, varType, varNum,
                    useMask, op == VAL_VARIABLE);

        case OP_EQ:
        case OP_NE:
        case OP_LT:
        case OP_GT:
        case OP_LE:
        case OP_GE:
        case OP_OR:
        case OP_AND:
        case OP_ADD:
        case OP_SUB:
        case OP_MULT:
        case OP_DIV:
            op1 = evalExpression(expr, servId, m, useMask);
            op2 = evalExpression(expr, servId, m, useMask);
            diff = op1 - op2;
            if (isnan(diff))
                return NAN;
            switch(op) {
            case OP_EQ:
            case OP_NE:
                b = IS_ZERO(diff);
                return (op == OP_EQ) ? b : !b;
            case OP_LE:
            case OP_GT:
                b = diff > EPSILON;
                return (op == OP_GT) ? b : !b;
            case OP_LT:
            case OP_GE:
                b = diff < -EPSILON;
                return (op == OP_LT) ? b : !b;
            case OP_OR:
                return IS_TRUE(op1) || IS_TRUE(op2);
            case OP_AND:
                return IS_TRUE(op1) && IS_TRUE(op2);
            case OP_ADD:
                return op1 + op2;
            case OP_SUB:
                return diff;
            case OP_MULT:
                return op1 * op2;
            case OP_DIV:
                return op1 / op2;
            }

        case OP_NOT:
            op1 = evalExpression(expr, servId, m, useMask);
            return isnan(op1) ? NAN : !IS_TRUE(op1);

        /* TODO: check for NaNs in remaining ops or drop them to save space */
        case OP_NEG:
            return -evalExpression(expr, servId, m, useMask);

        case OP_IN:
            i = getByte(expr++);
            op1 = evalExpression(expr, servId, m, useMask);
            while (i--) {
                diff = op1 - evalExpression(expr, servId, m, useMask);
                if (IS_ZERO(diff))
                    return 1.0f;
            }
            return 0.0f;

        case OP_IFELSE:
            op1 = evalExpression(expr, servId, m, useMask);
            if (IS_TRUE(op1)) {
                ret = evalExpression(expr, servId, m, useMask);
                evalExpression(expr, servId, m, useMask);
            } else {
                evalExpression(expr, servId, m, useMask);
                ret = evalExpression(expr, servId, m, useMask);
            }
            return ret;

        case OP_BETWEEN:
            ret = evalExpression(expr, servId, m, useMask);
            op1 = evalExpression(expr, servId, m, useMask);
            op2 = evalExpression(expr, servId, m, useMask);
            if (op1 < op2)
                return ret > op1 && ret < op2;
            else
                return ret > op2 && ret < op1;
        }

        return NAN;
    }

    float getVariableValue(Message &m, uint8_t servId,
            uint8_t varServId, Type t, uint8_t num,
            uint32_t &useMask, bool useCurrent) {
        /* Check if we have this variable's value cached */
        struct CachedValue *v = findVariable(varServId, t, num);

        /* Do the message's and the variable's ServiceIds match? */
        if (servId == varServId) {
            uint32_t value;

            /* Does the PUBLISH contain this variable? */
            if (m.find(t, num, &value)) {
                /* It does, mark it as in use */
                if (!v) {
                    v = cacheVariable(varServId, t, num);
                    if (!v)
                        return NAN;
                    v->value = NAN;
                }
                useMask |= (uint32_t) 1 << (v - valueCache);

                /* Check if we want to use its current value though */
                if (useCurrent)
                    return Message::toFloat(t, &value);
            }
        }

        return v ? v->value : NAN;
    }

    CachedValue *findVariable(uint8_t servId, Type type, uint8_t num) {
        struct CachedValue *v;
        for (v = valueCache; v < valueCache + ARRAY_SIZE(valueCache); v++)
            if (v->serviceId == servId && v->type == type && v->num == num)
                return v;
        return NULL;
    }

    CachedValue *cacheVariable(uint8_t servId, Type type, uint8_t num) {
        struct CachedValue *v;
        for (v = valueCache; v < valueCache + ARRAY_SIZE(valueCache); v++)
            if (v->serviceId == 0xff) {
                v->serviceId = servId;
                v->type = type;
                v->num = num;
                return v;
            }
        return NULL;
    }
};
/* vim: set sw=4 ts=4 et: */
