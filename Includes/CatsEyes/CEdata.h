#ifndef CEDATA_H_
#define CEDATA_H_

#include <vector>

// Objects
#define OBJECTS_CNT		21
#define X_ACCELERATION	0
#define Y_ACCELERATION	1
#define Z_ACCELERATION	2
#define X_ROTATION		3
#define Y_ROTATION		4
#define Z_ROTATION		5
#define X_MAGNETIC_FLUX	6
#define Y_MAGNETIC_FLUX	7
#define Z_MAGNETIC_FLUX	8
#define TEMPERATURE		9
#define PRESSURE		10
#define ALTITUDE		11
#define BATTERY_LVL		12
// Not for plots
#define DATE 			13
#define TIME 			14
#define SATELLITES 		15
#define LATITUDE 		16
#define LONGITUDE 		17
#define ALT_GPS 		18
#define SPEED 			19
#define ANGLE 			20

#define TRACKS_FILE		"TRACKS.TXT"
#define BUF_SIZE 		1024	//1024	// Taille minimum du buffer = 92 ( = taille maximale d'une acquisition)
#define FILE_NAME_SIZE 	31		// Taille maximale du nom d'un fichier

// Masks
#define GPS_MASK 		0x20
#define BAR_MASK		0X10
#define ACC_MASK		0x08
#define GYR_MASK		0X04
#define MAG_MASK		0x02
#define BAT_MASK		0X01

//#define FILE_LOCATION "/run/media/mgernet/0E3C-A754/"
#define FILE_LOCATION "Track/"

// Connection states
#define CONNECTED		true
#define DISCONNECTED	false

#define CMD_SIZE		4

/***************** Structures utilisées pour échanger les données *****************/

typedef struct 
{
	uint8_t sensors;				// Flags indiquant les capteurs utilisés lors de l'acquisition : | XXX | XXX | GPS | BAR | ACC | GYR | MAG | BAT |
	//unsigned int time_stamp;		// temps écoulé au moment de l'acquisition, en secondes
	float time_stamp;
	//uint32_t time_stamp;
	//uint8_t time_stamp;
}__attribute__((packed)) header_s;

typedef struct
{
	unsigned char year, month, day, hour, min, sec;
	char lat, lon; 					// 'E'/'W' and 'N'/'S'
	float latitude, longitude; 		// °
	unsigned char nb_sat;			// Number of satellites
	float alt_gps;					// Altitude (cm)
	char data_valid;				// 'A' or 'B'
	float speed;					// m/s
	float angle;					// °
}__attribute__((packed)) GPS_s;

typedef struct
{
	float pressure;					// hPa
	float temperature;				// °C
	float altitude;					// m
}__attribute__((packed)) BAR_s;

typedef struct
{
	float ax, ay, az;				// m/s²
}__attribute__((packed)) ACC_s;

typedef struct
{
	float gx, gy, gz;				// °/s
}__attribute__((packed)) GYR_s;

typedef struct
{
	float hx, hy, hz;				// uT
}__attribute__((packed)) MAG_s;

// Structure stockant l'ensemble des données d'une acquisition
typedef struct
{
	// header
	header_s header;
	// GPS
	GPS_s gps;
	// Barometer
	BAR_s bar;
	// Accelerometer
	ACC_s acc;
	// Gyroscope
	GYR_s gyr;
	// Magnetometer
	MAG_s mag;
	// Battery
	float voltage;	// V
}__attribute__((packed)) ACQ_s;

// Class utilisée pour stocker le nom des fichiers contenant les différents trajets effectués
// On attribue un indice à chaque fichier, par ordre d'apparition dans "Tracks.txt"
class TrackFiles
{
private:
	char** file_name;			// Tableau contenant le nom de chaque fichier
	int* number_of_points;		// Tableau contenant le nombre d'acquisitions présentes dans le fichier
	int number_of_files;

public:
	// Lit le fichier "Tracks.txt"
	TrackFiles();

	~TrackFiles();

	// Remplit la chaîne _file_name avec le nom du fichier à l'indice _file_index
	void getFileName(int _file_index, char* _file_name);

	// Retourne le nombre de points d'acquisition du fichier à l'indice _file_index
	int getNumberOfPoints(int _file_index);
	// Retourne le nombre de points d'acquisition, tous fichiers confondus
	int getNumberOfPoints();

	int getNumberOfFiles();
};


// Classe utilisée pour stocker les données d'une acquisition
class Acquisition
{
public:
	ACQ_s acq;

public:
	Acquisition(ACQ_s _acq);
	Acquisition();
	// Remplit les champs de la classe à partir d'un buffer. La position dans le buffer est indiquée par le curseur
	// Retourne la position du curseur pour la prochaine acquisition
	int loadAcquisition(unsigned char* _buf, int _cursor);

	// Remplit les champs de la classe à partir d'un fichier spécial
	// Retourne CONNECTED en cas de succès
	// Retourne DISCONNECTED si un prblème survient lors du transfert, probablement à cause d'une déconnexion
	//bool loadAcquisition(int _fd);

	float readBat();

	bool isUpdated(int _plot_object);

	float readValue(int _plot_object);
	void readDate(char* _date);
	void readTime(char* _time);
	void readLongitude(char* _longitude);
	void readLatitude(char* _latitude);
	GLfloat readTime();

	ACC_s readAcc();
	GYR_s readGyr();
};


// Classe utilisée pour stocker l'ensemble des données d'un trajet
class Track
{
private:
	FILE* fp;
	Acquisition* acq;
	unsigned int number_of_points;
	GLfloat time_duration;
	//GLfloat acq_frequency;

public:
	// Charge un fichier contenant les données stockées pendant un trajet
	Track(TrackFiles* _track_files);
	Track(int _file_index, TrackFiles* _track_files);
	Track(char* _file_name);

	GLfloat duration();
	//GLfloat frequency();
	Acquisition* acquisition(int i);

	unsigned int getNumberOfPoints();

	~Track();
};

/*
// Classe utilisée pour stocker les données au fur et à mesure qu'elle sont envoyées par le microcontroleur
class LiveTrack
{
public:
	char file_name[FILE_NAME_SIZE];
	int fd;
	bool connection_state;
	std::vector<Acquisition> acq;
	unsigned int number_of_points;

public:
	LiveTrack(char* _file_name);

	~LiveTrack();

	void readAcquisition();

	float readBat();

	ACC_s readAcc();

	Acquisition readLastAcq();
};
*/

// Le Sniffer initialise la liaison bluetooth, traite les données qui y passent, indiquent si de novuelles données sont disponibles, et retourne les dernières données reçues
class Sniffer
{
private:
	// Communication bluetooth :
	char file_name[FILE_NAME_SIZE];
	Acquisition last_acq;
	bool data_ready;
	bool connection_status;

	// Communication entre le processus père et fils
	pid_t pid;
	int p_mosi;
	int p_miso;

public:
	Sniffer();
	~Sniffer();

	void configureSensors(unsigned char _sensors);

	void connectBluetooth();

	void enterBurstMode();

	bool updateConnectionStatus();

	Acquisition getLastAcq();
	/*
	void initSniffer();

	bool enterBurstMode();

	bool parseBluetooth();
	*/
};


int set_interface_attribs (int fd, int speed, int parity);

void set_blocking (int fd, int should_block);

bool connect (int* fd, char* file_name);


#endif /* CEDATA_H_ */
