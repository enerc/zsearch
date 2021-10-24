#include "mapping_types.hpp"

namespace mapping {

const char * storage_type_names[] = {
	"storage_type_null",
	"storage_type_signed",
	"storage_type_unsigned",
	"storage_type_float16",
	"storage_type_float",
	"storage_type_double",
	"storage_type_geopoint",
	"storage_type_enum",
	"storage_type_signed_fixed_point",
	"storage_type_unsigned_fixed_point"
};

#define MACRO_S(X) {"int" #X "_t",index_type_signed, storage_type_signed,X,"signed "#X" bit size number" }
#define MACRO_U(X) {"uint" #X "_t",index_type_unsigned, storage_type_unsigned,X,"unsigned" #X "bit size number" }

const type_definition types[] = {
	{"enum" 		,index_type_enum, storage_type_enum, 			0,"enum" },
	{"boolean" 		,index_type_boolean, storage_type_signed,		1,"boolean" },
				MACRO_S(1),	MACRO_S(2),	MACRO_S(3),	MACRO_S(4),	MACRO_S(5),	MACRO_S(6),	MACRO_S(7),	MACRO_S(8),	MACRO_S(9),
	MACRO_S(10),MACRO_S(11),MACRO_S(12),MACRO_S(13),MACRO_S(14),MACRO_S(15),MACRO_S(16),MACRO_S(17),MACRO_S(18),MACRO_S(19),
	MACRO_S(20),MACRO_S(21),MACRO_S(22),MACRO_S(23),MACRO_S(24),MACRO_S(25),MACRO_S(26),MACRO_S(27),MACRO_S(28),MACRO_S(29),
	MACRO_S(30),MACRO_S(31),MACRO_S(32),MACRO_S(33),MACRO_S(34),MACRO_S(35),MACRO_S(36),MACRO_S(37),MACRO_S(38),MACRO_S(39),
	MACRO_S(40),MACRO_S(41),MACRO_S(42),MACRO_S(43),MACRO_S(44),MACRO_S(45),MACRO_S(46),MACRO_S(47),MACRO_S(48),MACRO_S(49),
	MACRO_S(50),MACRO_S(51),MACRO_S(52),MACRO_S(53),MACRO_S(54),MACRO_S(55),MACRO_S(56),MACRO_S(57),MACRO_S(58),MACRO_S(59),
	MACRO_S(60),MACRO_S(61),MACRO_S(62),MACRO_S(63),MACRO_S(64),MACRO_S(65),MACRO_S(66),MACRO_S(67),MACRO_S(68),MACRO_S(69),
	MACRO_S(70),MACRO_S(71),MACRO_S(72),MACRO_S(73),MACRO_S(74),MACRO_S(75),MACRO_S(76),MACRO_S(77),MACRO_S(78),MACRO_S(79),
	MACRO_S(80),MACRO_S(81),MACRO_S(82),MACRO_S(83),MACRO_S(84),MACRO_S(85),MACRO_S(86),MACRO_S(87),MACRO_S(88),MACRO_S(89),
	MACRO_S(90),MACRO_S(91),MACRO_S(92),MACRO_S(93),MACRO_S(94),MACRO_S(95),MACRO_S(96),MACRO_S(97),MACRO_S(98),MACRO_S(99),
	MACRO_S(100),MACRO_S(101),MACRO_S(102),MACRO_S(103),MACRO_S(104),MACRO_S(105),MACRO_S(106),MACRO_S(107),MACRO_S(108),MACRO_S(109),
	MACRO_S(110),MACRO_S(111),MACRO_S(112),MACRO_S(113),MACRO_S(114),MACRO_S(115),MACRO_S(116),MACRO_S(117),MACRO_S(118),MACRO_S(119),
	MACRO_S(120),MACRO_S(121),MACRO_S(122),MACRO_S(123),MACRO_S(124),MACRO_S(125),MACRO_S(126),MACRO_S(127),MACRO_S(128),

			    MACRO_U(1),	MACRO_U(2),	MACRO_U(3),	MACRO_U(4),	MACRO_U(5),	MACRO_U(6),	MACRO_U(7),	MACRO_U(8),	MACRO_U(9),
	MACRO_U(10),MACRO_U(11),MACRO_U(12),MACRO_U(13),MACRO_U(14),MACRO_U(15),MACRO_U(16),MACRO_U(17),MACRO_U(18),MACRO_U(19),
	MACRO_U(20),MACRO_U(21),MACRO_U(22),MACRO_U(23),MACRO_U(24),MACRO_U(25),MACRO_U(26),MACRO_U(27),MACRO_U(28),MACRO_U(29),
	MACRO_U(30),MACRO_U(31),MACRO_U(32),MACRO_U(33),MACRO_U(34),MACRO_U(35),MACRO_U(36),MACRO_U(37),MACRO_U(38),MACRO_U(39),
	MACRO_U(40),MACRO_U(41),MACRO_U(42),MACRO_U(43),MACRO_U(44),MACRO_U(45),MACRO_U(46),MACRO_U(47),MACRO_U(48),MACRO_U(49),
	MACRO_U(50),MACRO_U(51),MACRO_U(52),MACRO_U(53),MACRO_U(54),MACRO_U(55),MACRO_U(56),MACRO_U(57),MACRO_U(58),MACRO_U(59),
	MACRO_U(60),MACRO_U(61),MACRO_U(62),MACRO_U(63),MACRO_U(64),MACRO_U(65),MACRO_U(66),MACRO_U(67),MACRO_U(68),MACRO_U(69),
	MACRO_U(70),MACRO_U(71),MACRO_U(72),MACRO_U(73),MACRO_U(74),MACRO_U(75),MACRO_U(76),MACRO_U(77),MACRO_U(78),MACRO_U(79),
	MACRO_U(80),MACRO_U(81),MACRO_U(82),MACRO_U(83),MACRO_U(84),MACRO_U(85),MACRO_U(86),MACRO_U(87),MACRO_U(88),MACRO_U(89),
	MACRO_U(90),MACRO_U(91),MACRO_U(92),MACRO_U(93),MACRO_U(94),MACRO_U(95),MACRO_U(96),MACRO_U(97),MACRO_U(98),MACRO_U(99),
	MACRO_U(100),MACRO_U(101),MACRO_U(102),MACRO_U(103),MACRO_U(104),MACRO_U(105),MACRO_U(106),MACRO_U(107),MACRO_U(108),MACRO_U(109),
	MACRO_U(110),MACRO_U(111),MACRO_U(112),MACRO_U(113),MACRO_U(114),MACRO_U(115),MACRO_U(116),MACRO_U(117),MACRO_U(118),MACRO_U(119),
	MACRO_U(120),MACRO_U(121),MACRO_U(122),MACRO_U(123),MACRO_U(124),MACRO_U(125),MACRO_U(126),MACRO_U(127),MACRO_U(128),

	{"byte" 		,index_type_signed, storage_type_signed,			8,"signed byte" },
	{"short" 		,index_type_signed, storage_type_signed,			16,"signed short" },
	{"int" 			,index_type_signed, storage_type_signed,			32,"signed 32 bit int" },
	{"integer" 		,index_type_signed, storage_type_signed,			32,"signed 32 bit int" },
	{"int64"		,index_type_signed, storage_type_signed,			64,"signed 64 bit int" },
	{"float16" 		,index_type_float16, storage_type_float16,			16,"IEEE 754 half-precision binary floating-point format" },
	{"float" 		,index_type_float, storage_type_float,				32,"IEEE 754 single floating-point" },
	{"double" 		,index_type_double, storage_type_double,			64,"IEEE 754 double floating-point" },
	{"location" 	,index_type_geopoint, storage_type_geopoint,		64,"array of 2 float coordinates (lat/long)" },
	{"text" 		,index_type_string, storage_type_enum,				0,"string" },
	// not implemented yet {"binary" 		,index_type_binary, storage_type_enum,				0,"binary data" },
	{"date" 		,index_type_date, storage_type_unsigned,			32,"date" },
	{"date_nano" 	,index_type_date_nano, storage_type_unsigned,		64,"date in nano seconds" },
};

int getTypeDefinitionSize() {
	return sizeof(mapping::types)/sizeof(type_definition);
}

std::string getStorageName(storage_types t) {
	if (t <=storage_type_unsigned_fixed_point)  {
		return storage_type_names[(int)t];
	} else {
		return "not found";
	}
}

std::string getIndexTypeName(index_types t, uint32_t length) {
	for (const auto & type : types) {
		if (type.index_type == t && length == type.length) return type.json_name;
	}
	return "not found";
}


bool isSignedInt(storage_types s) {
    return (s==storage_type_signed || s ==storage_type_signed_fixed_point);
}

bool isUnsignedInt(storage_types s) {
    return (s==storage_type_unsigned || s ==storage_type_unsigned_fixed_point);
}

bool isInt(storage_types s) {
    return (isSignedInt(s) || isUnsignedInt(s));
}

bool isFloat16(storage_types s) {
    return (s==storage_type_float16);
}

bool isFloat(storage_types s) {
    return (s ==storage_type_float);
}

bool isDouble(storage_types s) {
    return (s==storage_type_double);
}

    [[maybe_unused]] bool isEnum(storage_types s) {
    return (s==storage_type_enum);
}

}


