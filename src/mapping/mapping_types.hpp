#pragma once
#include <cstdint>
#include <iostream>
#include <memory>

namespace indexes {
    class  IndexManager;
}

namespace mapping {

typedef enum {
    storage_type_unknown,
	storage_type_null,			// 1 bit
	storage_type_signed,		// 1..128 bits
	storage_type_unsigned,		// 1..128 bits
	storage_type_float16,		// 16 bits IEEE 754 half precision
	storage_type_float,			// 32 bits
	storage_type_double,		// 64 bits
	storage_type_geopoint,		// 2 float numbers
	storage_type_enum,			// variable length
	storage_type_signed_fixed_point,	// ex 8.50$ - scaled value must fit in a 64 bit number
	storage_type_unsigned_fixed_point	// ex 8.50$ - scaled value must fit in a 64 bit number
} storage_types;

extern const char * storage_type_names[];

typedef enum {
	index_type_none,
	index_type_enum,
	index_type_boolean,
	index_type_signed,
	index_type_unsigned,
	index_type_float16,
	index_type_float,
	index_type_double,
	index_type_geopoint,
	index_type_string,
	//index_type_binary,
	index_type_date,
	index_type_date_nano
} index_types;

typedef struct {
	const char *json_name;
	index_types index_type;
	storage_types type;
	uint8_t		length;
	const char *description;
} type_definition;
extern const type_definition types[];


typedef struct mapping_entry  {
    std::string             me;
    mapping::storage_types  s{storage_type_unknown};
    mapping::index_types   	t;
    uint32_t			   	flags{};
    uint32_t			   	length{};
    uint32_t				scale{};
    uint32_t				max_array_size{};
    std::string			   	format{};
    std::string             joinIndex{};
    std::string             joinThisKey{};
    std::string             joinRemoteKey{};
    std::string             joinValue{};
    //
    std::shared_ptr<indexes::IndexManager>  index;
    bool                    isFTDate{};
} mapping_entry;

[[nodiscard]] bool isSignedInt(storage_types ) ;
[[nodiscard]] bool isUnsignedInt(storage_types ) ;
[[nodiscard]] bool isInt(storage_types ) ;
[[nodiscard]] bool isFloat16(storage_types ) ;
[[nodiscard]] bool isFloat(storage_types ) ;
[[nodiscard]] bool isDouble(storage_types ) ;
[[maybe_unused]] [[nodiscard]] bool isEnum(storage_types ) ;

extern std::string getStorageName(storage_types t);
extern std::string getIndexTypeName(index_types t, uint32_t length);
extern int getTypeDefinitionSize();
}
