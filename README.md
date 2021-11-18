# ζsearch - The search engine for big data


## Resources
This project uses the following components:

- Simdjson : Parsing gigabytes of JSON per second [Simdjson][1]
- AquaHash: Fast Hashing With AES Intrinsics [AquaHash][2]
- BBHash: Minimal perfect hash function A. Limasset, G. Rizk, R. Chikhi, P. Peterlongo, Fast and Scalable Minimal Perfect Hashing for Massive Key Sets, SEA 2017 [BBHash][3]
- Hyrise C++ SQL Parser [Hyrise][6]. The parser has been modified to support 128 bits data types and geographic coordinates.
## Introduction
An other search engine AGAIN ?

 **zsearch** is designed for very large data sets (billions+ of documents). It's designed around the following considerations:
 
 - JSON format for documents,
 - SQL query langue
 - sql JOINS done at import time rather than at query time
 - strict data types
 - signed and unsigned integers from 1 bit to 128 bit included
 - float, double, half float and fixed point numbers
 - date and date nano
 - Spatial Reference System Identifier (SRID 4326)
 - X86_64 BMI2, AVX2 and AES instructions to process documents with minimal CPU instructions. Typical execution time for a comparison is around 2 CPU cycles per documents, which translates to 40 billions of documents processed per second on a 12 core CPU at 3.8 GHz.
 - SQL queries are translated to x86_64 optimized code before they get executed.
## Limitations
- JSON documents cannot be nested, but they can have arrays of the same data type
- When comparing two JSON arrays, they must have the same size (for now)
- Joins can only be done on non array data types (= non array foreign keys)
- Sorting is limited to two columns (for now)
## Platform
**zsearch** is only supported on Linux and on x86_64 hardware : Intel XEON and AMD Epyc/Threadripper. For non production purposes, it can run on recent Intel/AMD cpus.

For **zsearch** to work efficiently, its is recommended to have 32 cores/ 64 threads or more, enough RAM (see below) and very fast storage. SATA or Network cards are way too slow. The minimum storage requirement for the indexes is many M2.1 PCI-E 4.0 SSD or fast SAN access.

The memory footprint for a column in an index can be calculated as: number of documents in the index X (column bit size +2). For example, a 1 billion document file indexed as 18 bit unsigned int will use 2.5 Gb of RAM.

**zsearch** is designed to span threads with lowest possible shared data for optimum multi socket/CCX performance. NUMA should work well.

**zsearch** is using virtual memory almost evywhere. Enable 5 level paging on XEON processors - see [5 level paging white paper][4]
## Security
**zsearch** comes with a user port for "select" queries and an admin port for "drop/create/import". Please use a web server in front of **zsearch** to secure your data.

## How fast is **zsearch** ?
The reference platform is a Ryzen 3900x, 32Gb of RAM with 1 To M2.1 PCI-EX 4 SSD.
The reference dataset is the 2015 New York City taxi data base available at [Rally Taxi Database][5] : a 74 Gb JSON file with 165 346 692 documents.

- indexing 165 346 692 documents ~ **40 seconds** (4 million documents indexed per second)  vs 3 hours with mysql
- *select count(total_amount) from nyc_taxis where total_amount between 5.0 and 10.0* ~ **13 ms**, 59 173 567 documents counted
- *select count(total_amount) from nyc_taxis where st_Distance_Sphere(pickup_location,ST_SRID(Point(-74.86, 40.75),4326)) < 100.0* ~ **124 ms**,  2655 documents selected vs 13 min on mysql
- *select total_amount,pickup_datetime from nyc_taxis where pickup_datetime in('2015-01-01 00:34:42','2015-01-01 00:34:53') order by pickup_datetime,total_amount limit 5 offset 5* ~ **33ms**

 **zsearch** also provides fast maths calculations on columns. For example
 
 - *select count(total_amount) from nyc_taxis where 2.23\*total_amount+3.14159 < tip_amount\*1.356* ~ **50 ms**, 82140 documents selected vs 3min on mysql
 
 - *select avg(total_amount) from nyc_taxis where total_amount >= 1* ~ **190ms**, average computed over 165 157 620 documents.

A second dataset of 4 194 304 string documents is randomly created (use zsearch --check to create the test file).

- *select count(text2) from english_words where text2  like '%others%'* ~ **35ms**, 120 millions of strings scanned per second.
- *select count(text2) from english_words where text2  like 'season tell fun%why once making life ' ~ **13ms**

## Professional services
If you need professional services to speed up your project or to build new search features, please run zsearch --contact to get the informations.

## Documentation
### Datatypes
- Datatypes are one of *int1_t .. int128_t, uint1_t .. uint128_t, byte, short, int, integer, int64, float16, float, double, location, text, date, date_nano, enum, boolean*
- Scale is used for fixed point data. For example a currency field should use scale=2, so 10.25$ will be internally stored as 1025. Fixed point calculations are perfectly rounded.
- Format is used to format dates - example "%F %T"
- Arrays should be described by their maximum length - example "max_array_size" : 10 for a 10 values array
- Joins are defined by
     - the index name of imported data - "index"
     - the key used in this document - "this_key"
     - the key used in the imported document - "join_key"
     - the column which is imported - "value"
     - example: "vendor_name": { "type": "join" , "index" : "taxi_trip_type", "this_key": "trip_type", "join_key" : "id" , "value" : "name"}
     
The best is to have a look at file 'queries.py' which gives a good example of supported syntax. 

Index definition can be obtained from a data file with  /?genDefinition=file.json|lz4. See queries.py for details.

### JSON format
There are two suported JSON file format for imports:

- File with one JSON document per line without comma separator between lines:

{"id":1,"temp": 27}

{"id":2,"temp": 35}

{"id":5,"temp": 24}

- Lz4 JSON compressed file. The internal format is the same as above. 

Lz4 is the only supported format as it's the only one that can be decoded at GB/s speed.

### Used file space
Files are memory mapped. The size reported by 'ls -l' is the virtual size. To get the physical used size use 'ls -ls' or 'du -sk'

Documents are stored in chunks of 65536 documents.

When documents are removed, a chunk will be freed only when there are no documents inside. In any cases "holes"' in the file are not stored on the disk: they are unused in virtual memory.

### Only Linux ?
**zsearch** is targeted to the HPC/big data market where most servers are running Linux. If you are on Windows or on OSX, spin a docker container.

### No ARM?
Not fow now but will make sense as powerfull chips are becomming available.

### GPUs ?
GPUs don't have enough VRAM at the moment to handle large datasets. GPUs with many SSDs onboard may change the outlook, but more bandwidth is actually needed.

However, this is a viable option for custom functions that performs complex operations as the the time spent in processing will be greater than time spent in fetching data. If you have something in mind (for example complex pattern search in text/binary fields), you can contact me trought the email address given in 'zsearch --contact'.


### what ζsearch means?
*ζ* is the greek letter zeta (ζήτα) which is close to zetta. A zetta is 1 000 000 000 000 000 000 000. ζsearch has been designed to scale as much as possible, so one day one might be able search over such a huge number of documents.

This will probably require adding clustering which is not hard to implement given the choosen design.

### Builing zsearch
As simple as a cmake project: 

	mkdir build
	cd build 
	cmake .. 
	make -j #cores
	cp ../src/config.txt . (edit this file for your needs)

Dependancies: 

- c++ 17 compiler (gcc or clang)
- libmicrohttpd-dev (http server)
- libspdlog-dev (log management)
- libtbb2 and libtbb-dev (used for parallel quick sort algorithm)
- liblz4-dev (lz4 decompression during document import)
- python3 for running the checks


[1]:https://github.com/simdjson/simdjson
[2]:https://github.com/jandrewrogers/AquaHash
[3]:https://github.com/rizkg/BBHash
[4]:https://www.intel.com/content/dam/develop/public/us/en/documents/5-level-paging-white-paper.pdf
[5]:https://rally-tracks.elastic.co/nyc_taxis/documents.json.bz2
[6]:https://github.com/hyrise/sql-parser
