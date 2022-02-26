#pragma once
#include <string>

class GMLInstanceBase;
class GMLInstance;

const int GML_TYPE_REAL = 0;
const int GML_TYPE_STRING = 1;
const int GML_TYPE_ARRAY = 2;
const int GML_TYPE_POINTER = 3;
const int GML_TYPE_UNDEFINED = 5;
const int GML_TYPE_INT32 = 7;
const int GML_TYPE_INT64 = 10;
const int GML_TYPE_BOOL = 13;

struct GMLStringRef {
	const char* string;
	int refCountGML;
	int size;

	GMLStringRef(const char* str) {
		string = str;
		size = std::strlen(str);
		refCountGML = 1;
	}

	void free() {
		if (string != NULL) {
			delete[] string;
		}
	}
};

#pragma pack(push, 4)
struct GMLVar {
	union {
		int valueInt32; // int32
		long long valueInt64; // int64
		double valueReal; // number
		GMLStringRef* valueString; // string
		void* valueArray; // array
		void* valuePointer; // ptr
	};
	int flags; // Not sure what this is used for
	int	type;

	inline void setReal(double value) {
        freeValue();
		type = GML_TYPE_REAL;
		valueReal = value;
	}

	inline void setInt32(int value) {
        freeValue();
		type = GML_TYPE_INT32;
		valueInt32 = value;
	}

	inline void setInt64(long long value) {
        freeValue();
		type = GML_TYPE_INT64;
		valueInt64 = value;
	}

	inline void setBool(bool value) {
        freeValue();
		type = GML_TYPE_BOOL;
		valueReal = value ? 1 : 0;
	}

    inline void setString(const char* value) {
        freeValue();
        type = GML_TYPE_STRING;
        valueString = new GMLStringRef(_strdup(value));
    }

    inline void setUndefined() {
        freeValue();
        type = GML_TYPE_UNDEFINED;
        valuePointer = NULL;
    }

	double getReal() {
		switch (type) {
			case GML_TYPE_REAL:
			case GML_TYPE_BOOL:
				return valueReal;
			case GML_TYPE_INT32:
				return (double)valueInt32;
			case GML_TYPE_INT64:
				return (double)valueInt64;
			default:
				return 0;
		}
	}

	const char* getCString() {
		if (type == GML_TYPE_STRING) {
			if (valueString->string != NULL) {
				return valueString->string;
			}
		}
		return "";
	}

	inline std::string getString() {
		return std::string(getCString());
	}

	GMLVar() {
		type = GML_TYPE_UNDEFINED;
		valuePointer = NULL;
	}

	GMLVar(double value) { setReal(value); }
	GMLVar(float value) { setReal(value); }
	GMLVar(int value) { setInt32(value); }
	GMLVar(long long value) { setInt64(value); }
	GMLVar(bool value) { setReal(value ? 1 : 0); }

	GMLVar(const char* value) {
		type = GML_TYPE_STRING;
		valueString = new GMLStringRef(_strdup(value));
	}

	GMLVar(std::string value) {
		type = GML_TYPE_STRING;
		valueString = new GMLStringRef(_strdup(value.c_str()));
	}

	std::string toString() {
		switch (type) {
			case GML_TYPE_REAL:
			case GML_TYPE_BOOL:
				return std::to_string(valueReal);
			case GML_TYPE_INT32:
				return std::to_string(valueInt32);
			case GML_TYPE_INT64:
				return std::to_string(valueInt64);
			case GML_TYPE_POINTER:
				return "*" + std::to_string((INT64)valuePointer);
			case GML_TYPE_UNDEFINED:
				return "undefined";
			case GML_TYPE_STRING:
				return getString();
			case GML_TYPE_ARRAY:
				return "array";
			default:
				return "unkonwn";
		}
	}

	inline bool truthy() {
		// Returns whether the value casts to true in GML
		return getReal() > 0.5;
	}

	inline void freeValue() {
		if (type == GML_TYPE_STRING) {
			valueString->free();
			delete valueString;
			type = GML_TYPE_UNDEFINED;
			valuePointer = NULL;
		}
	}

};
#pragma pack(pop)

