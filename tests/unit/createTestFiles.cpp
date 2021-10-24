#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include "../../src/common/simdjson.h"
#include "../../config.h"

using namespace std;
using namespace simdjson;

//
// New York taxis
//
#if 0
{
  "mappings": {
    "properties": {
		"total_amount": { "type": "int30_t", "scale" : 2},
		"improvement_surcharge": { "type": "uint5_t", "scale": 1 },
		"pickup_location": { "type": "location"},
		"pickup_datetime": { "type": "date", "format" : "%F %T"},
		"trip_type": { "type": "enum"},
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
		}
	}
}
#endif
static const char* mysqlIndexDefinition = R"MYSQL_INDEX_DEF(
create table nyc_taxis (
    total_amount DECIMAL(10,2),
    improvement_surcharge float,
    pickup_location POINT NOT NULL SRID 4326,
    pickup_datetime datetime,
    trip_type smallint,
    dropoff_datetime datetime,
    rate_code_id smallint,
    tolls_amount float,
    dropoff_location  POINT NOT NULL SRID 4326,
    passenger_count smallint,
    fare_amount float,
    extra float,
    trip_distance float,
    tip_amount float,
    store_and_fwd_flag varchar(1),
    payment_type smallint,
    mta_tax float
);
)MYSQL_INDEX_DEF";

static void mysqlCreateTaxisFile() {
    cout << "Create New York taxi database (MySql) test file...." << endl;
    static const char*N = "null";
    FILE * fp;
    size_t len = 65536;
    char * line = (char*)malloc(len);
    ssize_t read;

    fp = fopen("tests/taxis.json", "r");
    if (fp == nullptr) {
        cout << "tests/taxis.json Not found" << endl;
        return;
    }

    FILE * fo = fopen("tests/taxis.sql","w");
    fprintf(fo,"use bench;\n");
    fprintf(fo,"drop table IF EXISTS nyc_taxis;\n");
    fprintf(fo,"%s\n",mysqlIndexDefinition);
    fprintf(fo,"commit;\n");
    fprintf(fo,"SET autocommit=0;\n");

    map<string,string> m;

    int Z = 0;
    dom::parser parser;

    while (((read = getline(&line, &len, fp)) != -1)) {
        m.clear();
        simdjson::dom::document_stream docs;
        auto error = parser.parse_many(line,read,dom::DEFAULT_BATCH_SIZE).get(docs);
        if (error) {
            cout << "Parse error " << line << endl;
        }

        for (auto doc : docs) {
            if (doc.is_object()) {
                for (const auto &e : doc.get_object()) {
                    stringstream ss;
                    ss << e.value;
                    string s = ss.str();

                    replace(s.begin(),s.end(),'[','(');
                    replace(s.begin(),s.end(),']',')');
                    replace(s.begin(),s.end(),'"','\'');
                    //cout << e.key.data() << " " << s << endl;
                    m[e.key.data()] = s;
                }

                stringstream sql;
                fprintf(fo,"insert into nyc_taxis values(");
                fprintf(fo,"%s,",(m.find("total_amount") == m.end() ? N : (m.at("total_amount")).c_str()));
                fprintf(fo,"%s,",(m.find("improvement_surcharge") == m.end() ? N : m.at("improvement_surcharge")).c_str());
                fprintf(fo,"%s,",(m.find("pickup_location") == m.end() ? N : "ST_SRID(Point"+m.at("pickup_location")+",4326)").c_str());
                fprintf(fo,"%s,",(m.find("pickup_datetime") == m.end() ? N : m.at("pickup_datetime")).c_str());
                fprintf(fo,"%s,",(m.find("trip_type") == m.end() ? N : m.at("trip_type")).c_str());
                fprintf(fo,"%s,",(m.find("dropoff_datetime") == m.end() ? N : m.at("dropoff_datetime")).c_str());
                fprintf(fo,"%s,",(m.find("rate_code_id") == m.end() ? N : m.at("rate_code_id")).c_str());
                fprintf(fo,"%s,",(m.find("tolls_amount") == m.end() ? N : m.at("tolls_amount")).c_str());
                fprintf(fo,"%s,",(m.find("dropoff_location") == m.end() ? N : "ST_SRID(Point"+m["dropoff_location"]+",4326)").c_str());
                fprintf(fo,"%s,",(m.find("passenger_count") == m.end() ? N : m.at("passenger_count")).c_str());
                fprintf(fo,"%s,",(m.find("fare_amount") == m.end() ? N : m.at("fare_amount")).c_str());
                fprintf(fo,"%s,",(m.find("extra") == m.end() ? N : m.at("extra").c_str()));
                fprintf(fo,"%s,",(m.find("trip_distance") == m.end() ? N : m.at("trip_distance")).c_str());
                fprintf(fo,"%s,",(m.find("tip_amount") == m.end() ? N : m.at("tip_amount")).c_str());
                fprintf(fo,"%s,",(m.find("store_and_fwd_flag") == m.end() ? N : m.at("store_and_fwd_flag")).c_str());
                fprintf(fo,"%s,",(m.find("payment_type") == m.end() ? N : m.at("payment_type")).c_str());
                fprintf(fo,"%s",(m.find("mta_tax") == m.end() ? N : m.at("mta_tax")).c_str());
                fprintf(fo,");\n");
            } else {
                cout << "Doc not an object.... !" << line << "! " << line[len-2] << endl;
            }
            m.clear();
        }

        Z++;
        if (Z % 10000 == 9999) {
            fprintf(fo,"commit; \n");
            cout << Z << "/165346692" << endl;
        }
    }
    fprintf(fo,"commit; \n");

    fclose(fp);
    fclose(fo);
}

//
// Join with trip_type
//
static void mysqlCreateTripTypeTestFile() {
    cout << "Create trip type MySql test file...." << endl;
    stringstream s;
    s << "use bench;" << std::endl;;
    s << "drop table IF EXISTS  trip_type;" << std::endl;;
    s << "create table trip_type (id INT, type varchar(256));" << std::endl;
    s << "commit;" << std::endl;
    s << "insert into trip_type values(0,'Unknown');\n";
    s << "insert into trip_type values(1,'City trip');\n";
    s << "insert into trip_type values(2,'State trip');\n";
    s << "commit;" << std::endl;
    FILE * fo = fopen("tests/trip_type.sql","w");
    fwrite(s.str().c_str(),s.str().size(),1,fo);
    fclose(fo);
}

static void createTripTypeTestFile() {
    cout << "Create trip type JSON test file...." << endl;
    stringstream s;
    s << R"({"id":0,"name":"Unknown"})" << endl;
    s << R"({"id":1,"name":"City trip"})" << endl;
    s << R"({"id":2,"name":"State trip"})" << endl;
    FILE * fo = fopen("tests/trip_type.json","w");
    fwrite(s.str().c_str(),s.str().size(),1,fo);
    fclose(fo);
}

//
// English words
//
static const char * COMMON_ENGLISH_WORDS[] = {
        "the","and","to","of","a","in","is","that","for","I","you","it","with","on","as","are","be","this","was","have","or","at","not","your","from",
        "we","by","will","can","but","they","an","he","all","has","if","their","one","do","more","n t","my","his","so","there","about","which","when",
        "what","out","up","our","who","also","had","time","some","would","were","like","been","just","her","new","other","them","she","people","these",
        "no","get","how","me","into","than","only","its","most","may","any","many","make","then","well","first","very","over","now","could","after",
        "even","because","us","said","good","way","two","should","work","use","through","see","know","did","much","where","years","need","him","back",
        "such","those","being","day","take","while","here","before","does","great","year","go","help","want","really","think","best","life","each",
        "made","right","world","business","home","own","down","still","used","find","around","going","every","both","last","off","too","same","information",
        "little","another","look","few","long","part","since","things","place","am","between","during","different","must","come","using","however","without",
        "high","why","something","online","system","better","three","never","always","love","say","might","next","company","state","number","again","free",
        "lot","under","family","found","within","give","set","school","important","water","able","keep","got","sure","end","money","service","small","put",
        "experience","having","once","available","health","support","often","including","days","away","old","area","feel","read","show","big","against","thing",
        "order","program","though","city","group","services","site","making","course","point","children","times","team","game","along","let","house","today",
        "body","working","case","man","real","provide","care","public","top","looking","several","start","less","process","become","actually","local","together",
        "person","change","book","enough","getting","week","power","until","market","fact","god","food","students","full","women","community","name","second","data",
        "government","says","others","ever","yet","research","done","left","far","large","called","doing","already","development","social","open","possible","side",
        "play","means","needs","try","came","ca","based","hard","thought","products","national","quality","level","live","design","makes","project","line","night",
        "least","whether","job","car","example","include","following","given","website","past","plan","offer","buy","call","went","simply","hand","music","easy",
        "problem","men","country","took","four","members","form","personal","control","energy","room","head","pay","create","run","kind","credit","almost","believe",
        "quite","mind","law","early","comes","states","usually","companies","web","taking","started","later","although","story","per","future","known","someone",
        "across","rather","young","whole","special","everything","months","anything","training","url","bit","seen","product","american","please","management",
        "cost","either","light","university","face","due","nothing","human","event","history","probably","friends","learn","current","tell","general","price",
        "list","type","building","industry","bad","check","everyone","office","idea","internet","news","million","video","among","air","especially","told","results",
        "post","hours","international","center","understand","above","addition","major","education","white","particular","problems","media","according","upon",
        "page","continue","black","study","issues","inside","technology","five","value","further","access","reason","short","true","simple","natural","amount",
        "search","result","taken","main","heart","space","financial","ago","trying","question","living","likely","interest","various","insurance","common",
        "move","child","yourself","report","certain","share","single","close","instead","bring","works","age","s","season","hope","coming","areas","ask",
        "medical","low","games","turn","key","party","add","month","seems","view","fun","matter","words","needed"
};

#if 0
{
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
#endif

inline void createCommonEnglishWordTestLine(stringstream &aLine) {
    const int S = sizeof(COMMON_ENGLISH_WORDS) / sizeof(char*);
    aLine << "{";
    aLine << "\"name\": \"" << COMMON_ENGLISH_WORDS[rand()%S] << " " << COMMON_ENGLISH_WORDS[rand()%S] << "\",";
    aLine << "\"age\": " << rand()%100 << ",";
    aLine << "\"address\": \"" << rand()%1000 << " " << "street " << COMMON_ENGLISH_WORDS[rand()%S] << "\",";
    aLine << "\"text1\": \"";
    for (int i= 0; i <9 ; i++) {
        aLine << COMMON_ENGLISH_WORDS[rand()%S] << " ";
    }
    aLine << COMMON_ENGLISH_WORDS[rand()%S];
    aLine << "\",";
    aLine << "\"text2\": \"";
    for (int i= 0; i <50; i++) {
        aLine << COMMON_ENGLISH_WORDS[rand()%S] << " ";
    }
    aLine << "\",";
    aLine << "\"tags\":[\"" << COMMON_ENGLISH_WORDS[rand()%S] << "\",\"" << COMMON_ENGLISH_WORDS[rand()%S] << "\",\"" << COMMON_ENGLISH_WORDS[rand()%S] << "\"]";
    aLine << "}\n";
}

static const char* mysqlIndexDefinitionW = R"MYSQL_INDEX_DEFW(
    create table english_words (
        name VARCHAR(255),
        age TINYINT,
        address VARCHAR(1024),
        text1  VARCHAR(256),
        text2  VARCHAR(2048)
    );
)MYSQL_INDEX_DEFW";

inline void mysqlCreateCommonEnglishWordTestLine(stringstream &aLine) {
    const int S = sizeof(COMMON_ENGLISH_WORDS) / sizeof(char*);
    aLine << "insert into " << "english_words" << " values('";
    aLine << COMMON_ENGLISH_WORDS[rand()%S] << " " << COMMON_ENGLISH_WORDS[rand()%S] << "',";
    aLine << rand()%100 << ",'";
    aLine << rand()%1000 << " " << "street " << COMMON_ENGLISH_WORDS[rand()%S] << "','";
    for (int i= 0; i <9 ; i++) {
        aLine << COMMON_ENGLISH_WORDS[rand()%S] << " ";
    }
    aLine << COMMON_ENGLISH_WORDS[rand()%S];
    aLine << "','";
    for (int i= 0; i <50; i++) {
        aLine << COMMON_ENGLISH_WORDS[rand()%S] << " ";
    }
    aLine << "'";
    rand();rand();rand();
    //aLine << "\"tags\":[\"" << COMMON_ENGLISH_WORDS[rand()%S] << "\",\"" << COMMON_ENGLISH_WORDS[rand()%S] << "\",\"" << COMMON_ENGLISH_WORDS[rand()%S] << "\"]";
    aLine << ");\n";
}

static void createCommonEnglishWordTestFile() {
    cout << "Create english words JSON test file...." << endl;
    stringstream s;
    srand(0);
    for (int i=0; i< CHUNK_SIZE*64; i++) {
        createCommonEnglishWordTestLine(s);
    }
    FILE *fo = fopen("tests/english_words.json","w");
    fwrite(s.str().c_str(),s.str().size(),1,fo);
    fclose(fo);
}

static void mysqlCreateCommonEnglishWordTestFile() {
    cout << "Create english words MySql test file...." << endl;
    stringstream s;
    s << "use bench;" << std::endl;;
    s << "drop table IF EXISTS english_words;" << std::endl;;
    s << mysqlIndexDefinitionW << std::endl;
    s << "commit;" << std::endl;
    s << "SET autocommit=0;" << std::endl;;

    srand(0);
    for (int i=0; i< CHUNK_SIZE*64; i++) {
        mysqlCreateCommonEnglishWordTestLine(s);
    }
    s << "commit;"<< std::endl;;
    FILE * fo = fopen("tests/english_words.sql","w");
    fwrite(s.str().c_str(),s.str().size(),1,fo);
    fclose(fo);
}

//
// station temperatures
//
#if 0
{
    "mappings": {
        "properties": {
            "id": { "type": "int4_t"},
            "temp": { "type": "int8_t"}
        }
    }
}
#endif
static void createStationTemperatureTestFile() {
    cout << "Create station temperature test file...." << endl;
    stringstream s;
    s << R"({"id":1,"temp": 27})" << endl;
    s << R"({"id":2,"temp": 35})" << endl;
    s << R"({"id":5,"temp": 24})" << endl;
    FILE * fo = fopen("tests/stationsTemperature.json","w");
    fwrite(s.str().c_str(),s.str().size(),1,fo);
    fclose(fo);
}

//
// Stations
//
#if 0
{
    "mappings": {
        "properties": {
            "id": { "type": "uint8_t"},
            "names": { "type": "enum" , "max_array_size" : 10},
            "elevation": { "type": "uint8_t" , "max_array_size" : 2},
            "temperature": { "type": "join" , "index" : "temperature", "key": "id", "value" : "temp"}
            }
    }
}
#endif
static void createStationTestFile() {
    cout << "Create station test file...." << endl;
    stringstream s;
    s << R"({"id":1,"names": ["Chicago0","Chicago1"],"elevation":[35,45]})" << endl;
    s << R"({"id":2,"names":["Atlanta0","Atlanta1","Atlanta2"],"elevation":[135,145]})" << endl;
    s << R"({"id":5,"names":["New York0","New York1","New York2","New York3"],"elevation":[5,15]})" << endl;
    FILE * fo = fopen("tests/stations.json","w");
    fwrite(s.str().c_str(),s.str().size(),1,fo);
    fclose(fo);
}


#if 0
rate_codes
{
    "mappings": {
        "properties": {
            "id" : { "type": "text"},
            "description": { "type": "text"}
        }
    }
}
#endif
static void createRateCodesTestFile() {
    cout << "Create rate codes test file...." << endl;
    stringstream s;
    s << R"({"id":1,"description": "Trip type 1"})" << endl;
    s << R"({"id":2,"description": "Trip type 2"})" << endl;
    s << R"({"id":3,"description": "Trip type 3"})" << endl;
    s << R"({"id":4,"description": "Trip type 4"})" << endl;
    s << R"({"id":5,"description": "Trip type 5"})" << endl;
    s << R"({"id":6,"description": "Trip type 6"})" << endl;
    s << R"({"id":99,"description": "Trip type 99"})" << endl;
    FILE * fo = fopen("tests/rateCodes.json","w");
    fwrite(s.str().c_str(),s.str().size(),1,fo);
    fclose(fo);
}

void taxis_below100() {
    cout << "Create taxi file where total_amount < 100" << endl;
    fstream fi,fo;
    fi.open("tests/taxis.json",ios::in);
    if (fi.fail()) {
        cerr << "taxis.json file NOT found" << endl;
        exit(0);
    }
    fo.open("tests/taxis_below100.json",ios::out);
    string tp;
    const char *k="\n";
    while(getline(fi, tp)){
        size_t i = tp.find("total_amount");
        i+= 12;
        while (tp.at(i) != ':') i++;
        i++;
        double g;
        sscanf(&tp.at(i),"%lf",&g);
        if (g < 100) {
            fo.write(tp.c_str(),tp.size());
            fo.write(k,1);
        }

    }
    fi.close();
    fo.close();
}

void taxis_below1() {
    cout << "Create taxi file where total_amount < 1" << endl;
    fstream fi,fo;
    fi.open("tests/taxis.json",ios::in);
    if (fi.fail()) {
        cerr << "taxis.json file NOT found" << endl;
        exit(0);
    }
    fo.open("tests/taxis_below1.json",ios::out);
    string tp;
    const char *k="\n";
    while(getline(fi, tp)){
        size_t i = tp.find("total_amount");
        i+= 12;
        while (tp.at(i) != ':') i++;
        i++;
        double g;
        sscanf(&tp.at(i),"%lf",&g);
        if (g < 100) {
            fo.write(tp.c_str(),tp.size());
            fo.write(k,1);
        }

    }
    fi.close();
    fo.close();
}

void createTestFiles() {
    createStationTemperatureTestFile();
    createStationTestFile();
    createRateCodesTestFile();
    createCommonEnglishWordTestFile();
    mysqlCreateCommonEnglishWordTestFile();
    createTripTypeTestFile();
    mysqlCreateTripTypeTestFile();
    taxis_below100();
    taxis_below1();
    //mysqlCreateTaxisFile();
}