#!/usr/bin/python3
import http.client
import time
import json
import urllib.parse
import os.path

conn = http.client.HTTPConnection("127.0.0.1:9091")


def query_function(q, r, s):
    start = time.time()
    qe = urllib.parse.quote(q)
    conn.request("GET", "/?query=" + qe)
    response = conn.getresponse()
    rr = response.read().decode('UTF-8')
    request_time = 1000*(time.time() - start)
    print("\033[38;5;250m", end='')
    if rr == r:
        print("\033[38;5;82mOK", end = "")
    else:
        print("\033[38;5;196mKO", end = "")
    print("\033[0m %.0lfms" % request_time, "/?query="+q, end = "   ")
    if rr != r:
        print("\033[38;5;243m"+rr+"\033[38;5;1m", end = "   ")
    print()


def create_index_function(q,r,s):
    start = time.time()
    a = json.dumps(s).encode('utf-8')
    conn.request("POST", "/create_index?name=" + q.replace(' ','%20'),a)
    response = conn.getresponse()
    rr = response.read().decode('UTF-8')
    request_time = 1000*(time.time() - start)
    print("\033[38;5;250m", end='')
    if rr == r:
        print("\033[38;5;82mOK", end = "")
    else:
        print("\033[38;5;196mKO", end = "")
    print("\033[0m %.0lfms" % request_time, "/create_index query="+q, end = "   ")
    if rr != r:
        print("\033[38;5;243m"+rr+"\033[38;5;0m", end = "   ")
    print()


if not os.path.exists("taxis.json"):
    print("File taxis.json is missing")
    print("Download taxi.json at https://rally-tracks.elastic.co/nyc_taxis/documents.json.bz2")
    print("Rename and uncompress this file. Put it or link it under bin/tests/taxis.json")
    exit(0)

if not os.path.exists("taxis.json.lz4"):
    print("File taxis.json.lz4 is missing")
    print("Download taxi.json at https://rally-tracks.elastic.co/nyc_taxis/documents.json.bz2")
    print("Uncompress this file and compress it again with lz4 and name it taxis.json.lz4");
    print("Put or link this files under bin/tests")
    exit(0)

if not os.path.exists("taxis_below100.json"):
    print("File taxis_below100.json")
    print("Run zsearch --check before to create tests files.")
    exit(0)

if not os.path.exists("taxis_below1.json"):
    print("File taxis_below1.json")
    print("Run zsearch --check before to create tests files.")
    exit(0)

q = "drop table taxis"
s = ""
r = "Drop index taxis failed. Table not found"
query_function(q, r, s)


q = "drop table if exists  nyc_taxis"
s = ""
r = ""
query_function(q, r, s)

q = "drop table if exists english_words"
s = ""
r = ""
query_function(q, r, s)

q = "drop table if exists taxi_trip_type"
s = ""
r = ""
query_function(q, r, s)

q = "drop table if exists rate_codes"
s = ""
r = ""
query_function(q, r, s)

q = "drop table if exists temperature"
s = ""
r = ""
query_function(q, r, s)

q = "drop table if exists stations"
s = ""
r = ""
query_function(q, r, s)


q = "english_words"
s = {
    "mappings": {
        "properties": {
            "name": { "type": "text"},
            "age": { "type": "uint7_t"},
            "address": { "type": "text"},
            "text1": { "type": "text"},
            "text2": { "type": "text"},
            "tags": { "type": "enum" , "max_array_size" : 10}
        }
    }
}
r = ""
create_index_function(q,r,s)

q = "taxi_trip_type"
s = {
    "mappings": {
        "properties": {
            "id" : { "type": "uint4_t"},
            "name": { "type": "text"}
        }
    }
}
r = ""
create_index_function(q,r,s)

q = "rate_codes"
s = {
    "mappings": {
        "properties": {
            "id" : { "type": "text"},
            "description": { "type": "text"}
        }
    }
}
r = ""
create_index_function(q,r,s)

q = "temperature"
s = {
    "mappings": {
        "properties": {
            "id": { "type": "int4_t"},
            "temp": { "type": "int8_t"}
        }
    }
}
r = ""
create_index_function(q,r,s)

q = "stations"
s = {
    "mappings": {
        "properties": {
            "id": { "type": "uint8_t"},
            "names": { "type": "enum" , "max_array_size" : 10},
            "elevation": { "type": "uint8_t" , "max_array_size" : 2},
            "temperature": { "type": "join" , "index" : "temperature", "this_key": "id", "join_key": "id", "value" : "temp"}
        }
    }
}
r = ""
create_index_function(q,r,s)

q = "nyc_taxis"
s = {
    "mappings": {
        "properties": {
            "total_amount": { "type": "int30_t", "scale" : 2},
            "improvement_surcharge": { "type": "uint5_t", "scale": 1 },
            "pickup_location": { "type": "location"},
            "pickup_datetime": { "type": "date", "format" : "%F %T"},
            "trip_type": { "type": "uint3_t"},
            "dropoff_datetime": { "type": "date", "format" : "%F %T"},
            "rate_code_id":  { "type": "enum" },
            "tolls_amount": { "type": "uint14_t", "scale": 2 },
            "dropoff_location": { "type": "location"},
            "passenger_count": { "type": "uint3_t" },
            "fare_amount": { "type": "int14_t", "scale": 2 },
            "extra": { "type": "int20_t", "scale": 2 },
            "trip_distance": { "type": "uint17_t", "scale": 2 },
            "tip_amount": { "type": "int30_t", "scale": 2 },
            "store_and_fwd_flag": { "type": "enum" },
            "payment_type": { "type": "enum" , "max_array_size" : 10},
            "mta_tax": { "type": "uint14_t", "scale": 2 },
            "vendor_id": { "type": "uint3_t" },
            "vendor_name": { "type": "join" , "index" : "taxi_trip_type", "this_key": "trip_type", "join_key" : "id" , "value" : "name"},
            "rate_code": { "type": "join" , "index" : "rate_codes", "this_key": "rate_code_id", "join_key" : "id", "value" : "description"}
        }
    }
}
r = ""
create_index_function(q,r,s)

q="import from json file 'tests/rateCodes.json' into rate_codes "
s = ""
r = "7 processed 0 failed 0 warnings 0 errors"
query_function(q, r, s)

q = "truncate english_words"
s = ""
r = ""
query_function(q, r, s)

q="import from json file 'tests/english_words.json' into english_words "
s = ""
r = "4194304 processed 0 failed 0 warnings 0 errors"
query_function(q, r, s)

q="import from json file 'tests/trip_type.json' into taxi_trip_type "
s = ""
r = "3 processed 0 failed 0 warnings 0 errors"
query_function(q, r, s)

q="truncate temperature"
s = ""
r = ""
query_function(q, r, s)

q="import from json file 'tests/stationsTemperature.json' into temperature"
s = ""
r = "3 processed 0 failed 0 warnings 0 errors"
query_function(q, r, s)

q="truncate stations "
s = ""
r = ""
query_function(q, r, s)

q="import from json file 'tests/stations.json' into stations "
s = ""
r = "3 processed 0 failed 0 warnings 0 errors"
query_function(q, r, s)

q = "truncate nyc_taxis"
s = ""
r = ""
query_function(q, r, s)

q="import from lz4 file 'tests/taxis.json.lz4' into nyc_taxis "
#q="import from json file 'tests/taxis.json' into nyc_taxis "
s = ""
r = "165346692 processed 0 failed 0 warnings 0 errors"
query_function(q, r, s)

q="select count(total_amount) from nyc_taxis where total_amount < 1"
#q="import from json file 'tests/taxis.json' into nyc_taxis "
s = ""
r = "{\"Count\":189072}\n"
query_function(q, r, s)

q="delete from nyc_taxis where total_amount < 1"
#q="import from json file 'tests/taxis.json' into nyc_taxis "
s = ""
r = ""
query_function(q, r, s)

#q="import from lz4 file 'tests/taxis_below1.lz4' into nyc_taxis "
q="import from json file 'tests/taxis_below1.json' into nyc_taxis "
s = ""
r = "189072 processed 0 failed 0 warnings 0 errors"
query_function(q, r, s)

q="select count(total_amount) from nyc_taxis where total_amount < 1"
s = ""
r = "{\"Count\":189072}\n"
query_function(q, r, s)

q="delete from nyc_taxis where total_amount < 100"
s = ""
r = ""
query_function(q, r, s)

q="import from json file 'tests/taxis_below100.json' into nyc_taxis "
s = ""
r = "165121598 processed 0 failed 0 warnings 0 errors"
query_function(q, r, s)

q="select count(total_amount) from nyc_taxis where total_amount < 1000"
s = ""
r = "{\"Count\":165346471}\n"
query_function(q, r, s)

q="select count(total_amount) from nyc_taxis where total_amount = 10.5"
s = ""
r = "{\"Count\":41318}\n"
query_function(q, r, s)

q="select count(total_amount) from nyc_taxis where total_amount < 10.5"
s = ""
r = "{\"Count\":67276933}\n"
query_function(q, r, s)

q="select count(total_amount) from nyc_taxis where total_amount <= 10.5"
s = ""
r = "{\"Count\":67318251}\n"
query_function(q, r, s)

q="select count(total_amount) from nyc_taxis where total_amount > 10.5"
s = ""
r = "{\"Count\":98028441}\n"
query_function(q, r, s)

q="select count(total_amount) from nyc_taxis where total_amount >= 10.5"
s = ""
r = "{\"Count\":98069759}\n"
query_function(q, r, s)

q="select count(total_amount) from nyc_taxis where total_amount != 10.5"
s = ""
r = "{\"Count\":165305374}\n"
query_function(q, r, s)

q="select count(total_amount) from nyc_taxis where total_amount in(10.5)"
s = ""
r = "{\"Count\":41318}\n"
query_function(q, r, s)

q="select count(total_amount) from nyc_taxis where total_amount in(10.5,11)"
s = ""
r = "{\"Count\":800960}\n"
query_function(q, r, s)

q="select count(total_amount) from nyc_taxis where total_amount in(10.5,11,5)"
s = ""
r = "{\"Count\":838013}\n"
query_function(q, r, s)

q="select count(total_amount) from nyc_taxis where total_amount in(10,11,5)"
s = ""
r = "{\"Count\":1124619}\n"
query_function(q, r, s)

q="select count(total_amount) from nyc_taxis where total_amount not in(10,11,5)"
s = ""
r = "{\"Count\":164222073}\n"
query_function(q, r, s)

q="select count(total_amount) from nyc_taxis where total_amount between 5 and 10"
s = ""
r = "{\"Count\":59173567}\n"
query_function(q, r, s)

q="select count(total_amount) from nyc_taxis where total_amount between 5.0 and 10.0"
s = ""
r = "{\"Count\":59173567}\n"
query_function(q, r, s)

q="select count(total_amount) from nyc_taxis where total_amount not between 5 and 10"
s = ""
r = "{\"Count\":106173125}\n"
query_function(q, r, s)

q="select count(total_amount) from nyc_taxis where 2*total_amount = 10"
s = ""
r = "{\"Count\":37053}\n"
query_function(q, r, s)

q="select count(total_amount) from nyc_taxis where 2.0*total_amount = 10"
s = ""
r = "{\"Count\":37053}\n"
query_function(q, r, s)

q="select count(total_amount) from nyc_taxis where 2.0*total_amount = 22/2-1"
s = ""
r = "{\"Count\":37053}\n"
query_function(q, r, s)

q="select count(total_amount) from nyc_taxis where 2.23*total_amount+3.14159 < tip_amount*1.356"
s = ""
r = "{\"Count\":82140}\n"
query_function(q, r, s)

q="select count(total_amount) from nyc_taxis where 1+total_amount = total_amount+1"
s = ""
r = "{\"Count\":165346692}\n"
query_function(q, r, s)

q="select count(total_amount) from nyc_taxis where 1.0+total_amount = total_amount+1.0"
s = ""
r = "{\"Count\":165346692}\n"
query_function(q, r, s)

q="select avg(total_amount) from nyc_taxis where total_amount < 10"
s = ""
r = "{\"Avg\":7.596956}\n"
query_function(q, r, s)


q="select avg(total_amount) from nyc_taxis where total_amount >= 1"
s = ""
r = "{\"Avg\":15.973913}\n"
query_function(q, r, s)

q="select sum(total_amount) from nyc_taxis where total_amount >= 10"
s = ""
r = "{\"Sum\":2161981962.01}\n"
query_function(q, r, s)

q="select count(total_amount) from nyc_taxis where total_amount/2 >= 0.5"
s = ""
r = "{\"Count\":165157620}\n"
query_function(q, r, s)

# CASE not supported yet
q="select total_amount, case when total_amount >= 1 then 'Above 1' when total_amount >= 2 then 'Above 2' else 'below 1' end from nyc_taxis where total_amount = 10"
s = ""
r = "Invalid type \"Operator\" for a selected columns. Must be a column or a function."
query_function(q, r, s)

# CONCAT not supported yet
q="SELECT CONCAT('SQL ', 'Tutorial ', 'is ', 'fun!') AS ConcatenatedString"
s = ""
r = "unrecognized operator type in \"SELECT CONCAT('SQL ', 'Tutorial ', 'is ', 'fun!') AS ConcatenatedString\""
query_function(q, r, s)

q="select count(total_amount) from nyc_taxis where st_Distance_Sphere(pickup_location,ST_SRID(Point(-73.86082458496094, 40.75779342651367),4326)) < 100.0"
s = ""
r = "{\"Count\":2655}\n"
query_function(q, r, s)

q="select count(total_amount) from nyc_taxis where st_Distance_Sphere(pickup_location,ST_SRID(Point(-74.86, 40.75),4326)) > 100.0"
s = ""
r = "{\"Count\":165346692}\n"
query_function(q, r, s)

q="select count(trip_type) from nyc_taxis where trip_type is null"
s = ""
r = "{\"Count\":146112999}\n"
query_function(q, r, s)

q="select count(trip_type) from nyc_taxis where trip_type is not null"
s = ""
r = "{\"Count\":19233693}\n"
query_function(q, r, s)

q="select count(trip_type) from nyc_taxis where total_amount = 3 and trip_type = 2"
s = ""
r = "{\"Count\":445}\n"
query_function(q, r, s)

q="select count(trip_type) from nyc_taxis where total_amount = 3 and trip_type = 2 or trip_type=1"
s = ""
r = "{\"Count\":18791687}\n"
query_function(q, r, s)

q="select count(trip_type) from nyc_taxis where total_amount = 3 and (trip_type = 2 or trip_type=1)"
s = ""
r = "{\"Count\":2392}\n"
query_function(q, r, s)

q="select count(total_amount) from nyc_taxis where total_amount = 3 and (trip_type is null)"
s = ""
r = "{\"Count\":1316}\n"
query_function(q, r, s)

q="select count(total_amount) from nyc_taxis where total_amount = 3 and (trip_type is not null)"
s = ""
r = "{\"Count\":2392}\n"
query_function(q, r, s)

q="select count(name) from english_words"
s = ""
r = "{\"Count\":4194304}\n"
query_function(q, r, s)

q="select count(name) from english_words where name  = 'day do'"
s = ""
r = "{\"Count\":23}\n"#
query_function(q, r, s)

q="select count(name) from english_words where name  != 'day do'"
s = ""
r = "{\"Count\":4194281}\n"
query_function(q, r, s)

q="select count(name) from english_words where name  like 'day do'"
s = ""
r = "{\"Count\":23}\n"
query_function(q, r, s)

q="select count(text1) from english_words where text1  like 'while%'"
s = ""
r = "{\"Count\":8356}\n"
query_function(q, r, s)

q="select count(text1) from english_words where text1  like '%program'"
s = ""
r = "{\"Count\":8468}\n"
query_function(q, r, s)

q="select count(text2) from english_words where text2  like '%others%'"
s = ""
r = "{\"Count\":400266}\n"
query_function(q, r, s)

q="select count(text2) from english_words where text2  like 'wor%ter '"
s = ""
r = "{\"Count\":523}\n"
query_function(q, r, s)

q="select count(text2) from english_words where text2  like 'season tell fun%why once making life '"
s = ""
r = "{\"Count\":1}\n"
query_function(q, r, s)

q="select count(text2) from english_words where age = 22"
s = ""
r = "{\"Count\":42123}\n"
query_function(q, r, s)

q="select count(total_amount) from nyc_taxis where total_amount < 1 and (total_amount > 3 or (total_amount >6 and total_amount<5))"
s = ""
r = "{\"Count\":0}\n"
query_function(q, r, s)

q="select count(total_amount) from nyc_taxis where total_amount < 1 and (total_amount > 3 or (total_amount >6 and total_amount<5))"
s = ""
r = "{\"Count\":0}\n"
query_function(q, r, s)

q="select count(pickup_datetime) from nyc_taxis where pickup_datetime < '2015-01-01 00:34:42'"
s = ""
r = "{\"Count\":18169}\n"
query_function(q, r, s)

q="select count(pickup_datetime) from nyc_taxis where pickup_datetime < '2015-01-01 00:34:42' and total_amount < 10.0"
s = ""
r = "{\"Count\":6543}\n"
query_function(q, r, s)

q="select count(pickup_datetime) from nyc_taxis where pickup_datetime between '2015-01-01 00:34:42' and '2015-01-02 00:34:42'"
s = ""
r = "{\"Count\":433079}\n"
query_function(q, r, s)

q="select count(pickup_datetime) from nyc_taxis where pickup_datetime in('2015-01-01 00:34:42','2015-01-01 00:34:53')"
s = ""
r = "{\"Count\":27}\n"
query_function(q, r, s)

q="select total_amount,pickup_datetime from nyc_taxis where pickup_datetime in('2015-01-01 00:34:42','2015-01-01 00:34:53') order by pickup_datetime,total_amount"
s = ""
r = '''{"total_amount":5.80,"pickup_datetime":"2015-01-01 00:34:42"}
{"total_amount":6.30,"pickup_datetime":"2015-01-01 00:34:42"}
{"total_amount":7.30,"pickup_datetime":"2015-01-01 00:34:42"}
{"total_amount":8.15,"pickup_datetime":"2015-01-01 00:34:42"}
{"total_amount":8.30,"pickup_datetime":"2015-01-01 00:34:42"}
{"total_amount":9.80,"pickup_datetime":"2015-01-01 00:34:42"}
{"total_amount":11.30,"pickup_datetime":"2015-01-01 00:34:42"}
{"total_amount":13.75,"pickup_datetime":"2015-01-01 00:34:42"}
{"total_amount":16.80,"pickup_datetime":"2015-01-01 00:34:42"}
{"total_amount":17.15,"pickup_datetime":"2015-01-01 00:34:42"}
{"total_amount":26.80,"pickup_datetime":"2015-01-01 00:34:42"}
{"total_amount":5.30,"pickup_datetime":"2015-01-01 00:34:53"}
{"total_amount":5.80,"pickup_datetime":"2015-01-01 00:34:53"}
{"total_amount":6.30,"pickup_datetime":"2015-01-01 00:34:53"}
{"total_amount":6.30,"pickup_datetime":"2015-01-01 00:34:53"}
{"total_amount":6.80,"pickup_datetime":"2015-01-01 00:34:53"}
{"total_amount":7.80,"pickup_datetime":"2015-01-01 00:34:53"}
{"total_amount":7.80,"pickup_datetime":"2015-01-01 00:34:53"}
{"total_amount":9.00,"pickup_datetime":"2015-01-01 00:34:53"}
{"total_amount":9.35,"pickup_datetime":"2015-01-01 00:34:53"}
{"total_amount":12.30,"pickup_datetime":"2015-01-01 00:34:53"}
{"total_amount":12.80,"pickup_datetime":"2015-01-01 00:34:53"}
{"total_amount":21.15,"pickup_datetime":"2015-01-01 00:34:53"}
{"total_amount":22.50,"pickup_datetime":"2015-01-01 00:34:53"}
{"total_amount":25.30,"pickup_datetime":"2015-01-01 00:34:53"}
{"total_amount":25.80,"pickup_datetime":"2015-01-01 00:34:53"}
{"total_amount":44.00,"pickup_datetime":"2015-01-01 00:34:53"}
'''
query_function(q, r, s)

q="select total_amount,pickup_datetime from nyc_taxis where pickup_datetime in('2015-01-01 00:34:42','2015-01-01 00:34:53') order by pickup_datetime,total_amount limit 5"
s = ""
r = '''{"total_amount":5.80,"pickup_datetime":"2015-01-01 00:34:42"}
{"total_amount":6.30,"pickup_datetime":"2015-01-01 00:34:42"}
{"total_amount":7.30,"pickup_datetime":"2015-01-01 00:34:42"}
{"total_amount":8.15,"pickup_datetime":"2015-01-01 00:34:42"}
{"total_amount":8.30,"pickup_datetime":"2015-01-01 00:34:42"}
'''
query_function(q, r, s)

q="select total_amount,pickup_datetime from nyc_taxis where pickup_datetime in('2015-01-01 00:34:42','2015-01-01 00:34:53') order by pickup_datetime,total_amount limit 5 offset 5"
s = ""
r='''{"total_amount":9.80,"pickup_datetime":"2015-01-01 00:34:42"}
{"total_amount":11.30,"pickup_datetime":"2015-01-01 00:34:42"}
{"total_amount":13.75,"pickup_datetime":"2015-01-01 00:34:42"}
{"total_amount":16.80,"pickup_datetime":"2015-01-01 00:34:42"}
{"total_amount":17.15,"pickup_datetime":"2015-01-01 00:34:42"}
'''
query_function(q, r, s)

q="select * from stations"
s = ""
r = '''{"id":1,"names":["Chicago0","Chicago1"],"elevation":[35,45],"temperature":27}
{"id":2,"names":["Atlanta0","Atlanta1","Atlanta2"],"elevation":[135,145],"temperature":35}
{"id":5,"names":["New York0","New York1","New York2","New York3"],"elevation":[5,15],"temperature":24}
'''
query_function(q, r, s)

q="select * from stations where temperature=35"
s = ""
r = '''{"id":2,"names":["Atlanta0","Atlanta1","Atlanta2"],"elevation":[135,145],"temperature":35}
'''
query_function(q, r, s)

q="select * from stations where names = 'Atlanta1'"
s = ""
r = '''{"id":2,"names":["Atlanta0","Atlanta1","Atlanta2"],"elevation":[135,145],"temperature":35}
'''
query_function(q, r, s)

q="select * from stations where elevation > 135"
s = ""
r = '''{"id":2,"names":["Atlanta0","Atlanta1","Atlanta2"],"elevation":[135,145],"temperature":35}
'''
query_function(q, r, s)

q="select * from stations where elevation between 45 and 145"
s = ""
r = '''{"id":1,"names":["Chicago0","Chicago1"],"elevation":[35,45],"temperature":27}
{"id":2,"names":["Atlanta0","Atlanta1","Atlanta2"],"elevation":[135,145],"temperature":35}
'''
query_function(q, r, s)

q="select * from stations where id in (1,2,5)"
s = ""
r = '''{"id":1,"names":["Chicago0","Chicago1"],"elevation":[35,45],"temperature":27}
{"id":2,"names":["Atlanta0","Atlanta1","Atlanta2"],"elevation":[135,145],"temperature":35}
{"id":5,"names":["New York0","New York1","New York2","New York3"],"elevation":[5,15],"temperature":24}
'''
query_function(q, r, s)
