#ifndef __NMEAPARSERDEFS_H__
#define __NMEAPARSERDEFS_H__


#define NMEAPARSER_MAX_SENTENCE_LENGTH 255 //MAX Sentence Length in bytes
#define NMEAPARSER_MAX_TOKENS 24 //Max number of data fields in one GPS Sentence

enum NMEA_SENTENCES_IDS {
	NMEAPARSER_GPGGA_ID = 0,
	NMEAPARSER_GPRMC_ID,
	NMEAPARSER_GPGSV_ID,
	NMEAPARSER_GPTXT_ID,
	NMEAPARSER_GPVTG_ID,
	NMEAPARSER_GPGSA_ID,
	NMEAPARSER_GPGLL_ID,
	NMEAPARSER_GPZDA_ID,
	NMEAPARSER_MAX_SENTENCES
};

/** GGA Sentences fields structure */
typedef struct {
	double        latitude;
	double        longitude;
	double        altitude;
	unsigned long time;
	int           satellites;
	int           quality;
	double        hdop;
	double        geoid;
}nmeaparser_gga_sentence;

/** RMC Sentences fields structure */
typedef struct {
	unsigned long time;
	char          warn;
	double        latitude;
	double        longitude;
	double        speed;
	double        course;
	unsigned long date;
	double        magvar;
}nmeap_rmc_sentence;

/** GSV Sentences fields structure */
typedef struct {
	unsigned int nomsg;
	unsigned int msgno;
	unsigned int  nosv;
	struct {
		int 	sv;
		int     elevation;
		int     azimuth;
		int 	cno;
	}satellite[64];
}nmeaparser_gsv_sentence;

/** TXT Sentences fields structure */
typedef struct {
	int 	number;
	struct {
		int		total;
		int		number;
		int		severity;
		char	message[255];
	}id[16];
}nmeaparser_txt_sentence;

/** VTG Sentences fields structure */
typedef struct {
	double        course;
	double        speedkn;
	double        speedkm;
}nmeaparser_vtg_sentence;

/** GSA Sentences fields structure */
typedef struct {
	char 	smode;
	int 	fs;
	int 	sv[12];
	float	pdop;
	float hdop;
	float vdop;
}nmeaparser_gsa_sentence;

typedef void (*nmeaparser_callback) (void * NmeaParserStruct, void * nmeaparser_data);

typedef struct {
	NMEA_SENTENCES_IDS NMEASentenceID;
	void * NmeaParserStruct;
	nmeaparser_callback nmeap_cb_func;
}NmeaParserGen;

#endif
