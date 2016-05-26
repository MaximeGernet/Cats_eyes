#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <fcntl.h>
#include <stdint.h>

#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <GL/glew.h>

#include <math.h>

#include "CEdata.h"

TrackFiles::TrackFiles()
{
	FILE* fp = NULL;
	int i, j, k;
	char buf[BUF_SIZE];
	char file[BUF_SIZE];

	strcpy(file, FILE_LOCATION);
	strcat(file, TRACKS_FILE);
	printf("Opening file %s\n", file);
	fp = fopen(file, "r");
	if(fp != NULL)
		printf("opened\n");

	number_of_files = 0;
	// Calcule du nombre de fichiers
	// On va lire le fichier ligne par ligne, sachant qu'il y a un nom de fichier par ligne 
	while(fgets(buf, sizeof buf, fp) != NULL)
		number_of_files++;

	printf("number of files: %d\n", number_of_files);
	// Allocation dynamique des tableaux
	file_name = new char* [number_of_files];
	for(i = 0; i < number_of_files; i++)
		file_name[i] = new char [FILE_NAME_SIZE + 1];

	number_of_points = new int [number_of_files];

	// On lit à nouveau le fichier pour remplir les tableaux
	rewind(fp);
	for(i = 0; i < number_of_files; i++)
	{
		fgets(buf, sizeof buf, fp);
		
		// On cherche la position de la virgule séparant le nom du fichier et sa taille
		j = 0;
		while(buf[j] != ',' && j < FILE_NAME_SIZE)
			j++;

		// On copie le nom du fichier
		strncpy(file_name[i], buf, j);
		file_name[i][j] = 0;

		// On copie la taille du fichier
		j++;
		k = j;
		while(buf[k] != '\n')
			k++;

		buf[k] = 0;
		number_of_points[i] = atoi(buf + j);
	}

	fclose(fp);

	for(i = 0; i < number_of_files; i++)
	{
		printf("%s %d\n", file_name[i], number_of_points[i]);
	}
	printf("done\n");
}

TrackFiles::~TrackFiles()
{
	//delete file_name;

	delete number_of_points;
}

void TrackFiles::getFileName(int _file_index, char* _file_name)
{
	strcpy(_file_name, file_name[_file_index]);
}

int TrackFiles::getNumberOfPoints(int _file_index)
{
	return number_of_points[_file_index];
}

int TrackFiles::getNumberOfPoints()
{
	int num_points = 0;
	for(int i = 0; i < number_of_files; i++)
	{
		num_points += getNumberOfPoints(i);
	}
	return num_points;
}

int TrackFiles::getNumberOfFiles()
{
	return number_of_files;
}













Track::Track(int _file_index, TrackFiles* _track_files)	// Chargement de la totalité des données enregistrées pendant un trajet
{
	char file[BUF_SIZE];
	char file_name[FILE_NAME_SIZE + 1];
	unsigned char buf[BUF_SIZE];
	int i;
	int ret;
	int cursor;
	int bytes_read;

	number_of_points = _track_files->getNumberOfPoints(_file_index);
	_track_files->getFileName(_file_index, file_name);

	strcpy(file, FILE_LOCATION);
	strcat(file, file_name);
	printf("Opening file %s\n", file);
	fp = fopen(file, "r");

	acq = new Acquisition [number_of_points];

	i = 0;
	do
	{
		ret = fread(buf, sizeof(char), BUF_SIZE, fp);
		//printf("\nNouveau buffer\n");
		cursor = 0;
		do
		{
			bytes_read = acq[i].loadAcquisition(buf, cursor);
			cursor += bytes_read;
			i++;
		}while(cursor < BUF_SIZE && i < number_of_points && bytes_read > 0);

		if(bytes_read == 0)	// La dernière acquisition n'est pas entièrement présente dans le buffer
		{
			fseek(fp, cursor - BUF_SIZE, SEEK_CUR);
			i--;
		}

	}while(ret == BUF_SIZE);

	// Estimating duration
/*
	// Estimating frequency
	acq_frequency = 0.0f;
	GLfloat t1, t2;
	t1 = acq[0].readTime();
	printf("t1: %f\n", t1);
	for(int i = 1; i < 64 && i < number_of_points; i++)
	{
		t2 = t1;
		t1 = acq[i].readTime();
		acq_frequency += 1.0f / (t1 - t2);
		printf("t1: %f\n", t1);
	}
	acq_frequency = acq_frequency / (float)(i + 1);
	*/
}

Track::Track(TrackFiles* _track_files)
{
	char file[BUF_SIZE];
	char** file_name;
	int* file_points;
	unsigned char buf[BUF_SIZE];
	int ret;
	int cursor;
	int bytes_read;
	int number_of_files;
	int total_points;
	int file_acq_cnt;
	int total_acq_cnt;

	number_of_files = _track_files->getNumberOfFiles();
	file_points = new int [number_of_files];
	file_name = new char* [number_of_files];
	for(int i = 0; i < number_of_files; i++)
	{
		file_name[i] = new char [BUF_SIZE];
		_track_files->getFileName(i, file_name[i]);
		file_points[i] = _track_files->getNumberOfPoints(i);
	}
	total_points = _track_files->getNumberOfPoints();
	acq = new Acquisition [total_points]; 	// Pour les trajets comportant beaucoup de points, il vaudrait mieux ne pas tout charger en RAM

	total_acq_cnt = 0;
	for(int i = 0; i < number_of_files; i++)
	{
		strcpy(file, FILE_LOCATION);
		strcat(file, file_name[i]);
		printf("Opening file %s\n", file);
		fp = fopen(file, "r");

		file_acq_cnt = 0;
		do
		{
			ret = fread(buf, sizeof(char), BUF_SIZE, fp);
			//printf("\nNouveau buffer\n");
			cursor = 0;
			do
			{
				bytes_read = acq[total_acq_cnt + file_acq_cnt].loadAcquisition(buf, cursor);
				cursor += bytes_read;
				file_acq_cnt++;
			}while(cursor < BUF_SIZE && file_acq_cnt < file_points[i] && bytes_read > 0);

			if(bytes_read == 0)	// La dernière acquisition n'est pas entièrement présente dans le buffer
			{
				fseek(fp, cursor - BUF_SIZE, SEEK_CUR);
				file_acq_cnt--;
			}

		}while(ret == BUF_SIZE);
		total_acq_cnt += file_acq_cnt;

		fclose(fp);
	}

	for(int i = 0; i < number_of_files; i++)
		delete file_name[i];
	delete file_name;
	delete file_points;

	number_of_points = total_acq_cnt;
	time_duration = acq[number_of_points - 1].readTime() - acq[0].readTime();
	//printf("t_end: %f; t_start: %f\n", acq[number_of_points - 1].readTime(), acq[0].readTime());
}

Track::~Track()
{
	delete acq;
}

unsigned int Track::getNumberOfPoints()
{
	return number_of_points;
}

GLfloat Track::duration()
{
	return time_duration;
}

/*
GLfloat Track::frequency()
{
	return acq_frequency;
}
*/

Acquisition* Track::acquisition(int i)
{
	return &(acq[i]);
}
/*
LiveTrack::LiveTrack(char* _file_name)			// Lecture du flux par bluetooth
{
	strcpy(file_name, _file_name);
	connection_state = connect(&fd, file_name);
	number_of_points = 0;
}

LiveTrack::~LiveTrack()
{
	if(connection_state == CONNECTED)
		close(fd);
}

void LiveTrack::readAcquisition()
{
	Acquisition new_acq;

	if(connection_state == CONNECTED)
	{
		if(new_acq.loadAcquisition(fd))			// Transmission success
		{
			acq.push_back(new_acq);
			number_of_points++;
		}
		else
		{
			connection_state = DISCONNECTED;
			close(fd);
		}
	}
	else 										// Connection attempt
	{
		connection_state = connect(&fd, file_name);
	}
}

float LiveTrack::readBat()
{
	return acq[number_of_points - 1].readBat();
}

ACC_s LiveTrack::readAcc()
{
	return acq[number_of_points - 1].readAcc();
}

Acquisition LiveTrack::readLastAcq()
{
	return acq[number_of_points - 1];
}
*/













Acquisition::Acquisition(ACQ_s _acq)
{
	acq = _acq;
}

Acquisition::Acquisition()
{
}

int Acquisition::loadAcquisition(unsigned char* _buf, int _cursor)
{
	int bytes_read = 0;

	if(sizeof(header_s) > BUF_SIZE - _cursor)
		return 0;

	memcpy(&acq.header, _buf + _cursor, sizeof(header_s));
	bytes_read += sizeof(header_s);

	//printf("\nsensors: %X\n", acq.header.sensors);
	//printf("time stamp: %f\n", acq.header.time_stamp);

	if((acq.header.sensors & 0x20) == 0x20)	// GPS
	{
		if(sizeof(GPS_s) > BUF_SIZE - bytes_read - _cursor)
			return 0;

		memcpy(&acq.gps, _buf + _cursor + bytes_read, sizeof(GPS_s));
		bytes_read += sizeof(GPS_s);
		
		//printf("data_valid: %c\n", acq.gps.data_valid);
		//printf("nb_sat: %u\n", acq.gps.nb_sat);
//		if(acq.gps.nb_sat != 0)
//		{
//			printf("date: %u/%u/20%u\n", acq.gps.day, acq.gps.month, acq.gps.year);
//			printf("time: %u:%u.%u\n", acq.gps.hour, acq.gps.min, acq.gps.sec);
//			printf("latitude: %f %c\n", acq.gps.latitude, acq.gps.lat);
//			printf("longitude: %f %c\n", acq.gps.longitude, acq.gps.lon);
//			printf("alt: %f\n", acq.gps.alt_gps);
//			printf("speed: %f\n", acq.gps.speed);
//			printf("angle: %f\n", acq.gps.angle);
//		}
		
	}

	if((acq.header.sensors & 0x10) == 0x10)	// BAR
	{
		if(sizeof(BAR_s) > BUF_SIZE - bytes_read - _cursor)
			return 0;

		memcpy(&acq.bar, _buf + _cursor + bytes_read, sizeof(BAR_s));
		bytes_read += sizeof(BAR_s);
	}

	if((acq.header.sensors & 0x08) == 0x08)	// ACC
	{
		if(sizeof(ACC_s) > BUF_SIZE - bytes_read - _cursor)
			return 0;

		memcpy(&acq.acc, _buf + _cursor + bytes_read, sizeof(ACC_s));
		bytes_read += sizeof(ACC_s);
	}

	if((acq.header.sensors & 0x04) == 0x04)	// GYR
	{
		if(sizeof(GYR_s) > BUF_SIZE - bytes_read - _cursor)
			return 0;

		memcpy(&acq.gyr, _buf + _cursor + bytes_read, sizeof(GYR_s));
		bytes_read += sizeof(GYR_s);
	}

	if((acq.header.sensors & 0x02) == 0x02)	// MAG
	{
		if(sizeof(MAG_s) > BUF_SIZE - bytes_read - _cursor)
			return 0;

		memcpy(&acq.mag, _buf + _cursor + bytes_read, sizeof(MAG_s));
		bytes_read += sizeof(MAG_s);
	}

	if((acq.header.sensors & 0x01) == 0x01)	// BAT
	{
		if(sizeof(float) > BUF_SIZE - bytes_read - _cursor)
			return 0;

		memcpy(&acq.voltage, _buf + _cursor + bytes_read, sizeof(float));
		bytes_read += sizeof(float);

		//printf("voltage: %f\n", acq.voltage);
	}

	return bytes_read;
}
/*
bool Acquisition::loadAcquisition(int _fd)
{
	unsigned char buf[BUF_SIZE];
	int n;
	unsigned int t;
	int byte_cnt, cursor;

	// First, a request is sent to the device (the device is requested to send its last acquisition)
	strncpy((char*)buf, "ACK", 3);
	write(_fd, buf, 3);

	// The device has 1 sec to send the acquisition. Otherwise, a conection timed out is triggered
	// To begin with, only the header is read since the total size of the data transmission can vary
	t = SDL_GetTicks();
	n = 0;
	while(n < sizeof(header_s) && SDL_GetTicks() - t < 5000)
	{
		n += read(_fd, (unsigned char*)(&header) + n, sizeof(header_s) - n);
	}

	if(n == sizeof(header_s))
	{
		printf("\nsensors: %X\n", header.sensors);
		printf("time_stamp: %f\n", header.time_stamp);
	}
	else
	{
		printf("ERROR: CONNECTION TIMED OUT\n");
		return DISCONNECTED;
	}
	
	// The remaining amount of bytes to be read is calculated
	byte_cnt = 0;
	if(header.sensors & GPS_MASK)
		byte_cnt += sizeof(GPS_s);
	if(header.sensors & BAR_MASK)
		byte_cnt += sizeof(BAR_s);
	if(header.sensors & ACC_MASK)
		byte_cnt += sizeof(ACC_s);
	if(header.sensors & GYR_MASK)
		byte_cnt += sizeof(GYR_s);
	if(header.sensors & MAG_MASK)
		byte_cnt += sizeof(MAG_s);
	if(header.sensors & BAT_MASK)
		byte_cnt += sizeof(float);

	// The remaining bytes are read
	n = 0;
	t = SDL_GetTicks();
	while(n < byte_cnt && SDL_GetTicks() - t < 1000)
	{
		n += read(_fd, buf + n, byte_cnt - n);
	}

	if(n != byte_cnt)
	{
		printf("ERROR: CONNECTION TIMED OUT\n");
		return DISCONNECTED;
	}

	// The fields of the class are filled
	cursor = 0;
	if(header.sensors & GPS_MASK)
	{
		memcpy(&gps, buf + cursor, sizeof(GPS_s));
		cursor += sizeof(GPS_s);

		printf("data_valid: %c\n", gps.data_valid);
		printf("nb_sat: %u\n", gps.nb_sat);
		if(gps.nb_sat != 0)
		{
			printf("date: %u/%u/20%u\n", gps.day, gps.month, gps.year);
			printf("time: %u:%u.%u\n", gps.hour, gps.min, gps.sec);
			printf("latitude: %f %c\n", gps.latitude, gps.lat);
			printf("longitude: %f %c\n", gps.longitude, gps.lon);
			printf("alt: %f\n", gps.alt_gps);
			printf("speed: %f\n", gps.speed);
			printf("angle: %f\n", gps.angle);
		}
	}

	if(header.sensors & BAR_MASK)
	{
		memcpy(&bar, buf + cursor, sizeof(BAR_s));
		cursor += sizeof(BAR_s);
		printf("tmp: %f °C\n", bar.temperature);
		printf("pressure: %f hPa\n", bar.pressure);
		printf("altitude: %f m\n", bar.altitude);
	}

	if(header.sensors & ACC_MASK)
	{
		memcpy(&acc, buf + cursor, sizeof(ACC_s));
		cursor += sizeof(ACC_s);
		printf("X accel: %f\n", acc.ax);
	}

	if(header.sensors & GYR_MASK)
	{
		memcpy(&gyr, buf + cursor, sizeof(GYR_s));
		cursor += sizeof(GYR_s);
		printf("gx: %f °/s\n", gyr.gx);
	}

	if(header.sensors & MAG_MASK)
	{
		memcpy(&mag, buf + cursor, sizeof(MAG_s));
		cursor += sizeof(MAG_s);
		printf("hx: %f uT\n", mag.hx);
		printf("hy: %f uT\n", mag.hy);
		printf("hz: %f uT\n", mag.hz);
	}

	if(header.sensors & BAT_MASK)
	{
		memcpy(&voltage, buf + cursor, sizeof(float));
		printf("voltage: %f\n", voltage*3.3*6.6*3.85/(4.4*5.0));
	}

	return CONNECTED;
}
*/

float Acquisition::readBat()
{
	return acq.voltage;
}

GLfloat Acquisition::readTime()
{
	return acq.header.time_stamp;
}

ACC_s Acquisition::readAcc()
{
	return acq.acc;
}

GYR_s Acquisition::readGyr()
{
	return acq.gyr;
}

bool Acquisition::isUpdated(int _plot_object)
{
	switch(_plot_object)
	{
		case X_ACCELERATION:
			if(acq.header.sensors & ACC_MASK)
				return true;
			break;
		case Y_ACCELERATION:
			if(acq.header.sensors & ACC_MASK)
				return true;
			break;
		case Z_ACCELERATION:
			if(acq.header.sensors & ACC_MASK)
				return true;
			break;
		case X_ROTATION:
			if(acq.header.sensors & GYR_MASK)
				return true;
			break;
		case Y_ROTATION:
			if(acq.header.sensors & GYR_MASK)
				return true;
			break;
		case Z_ROTATION:
			if(acq.header.sensors & GYR_MASK)
				return true;
			break;
		case X_MAGNETIC_FLUX:
			if(acq.header.sensors & MAG_MASK)
				return true;
			break;
		case Y_MAGNETIC_FLUX:
			if(acq.header.sensors & MAG_MASK)
				return true;
			break;
		case Z_MAGNETIC_FLUX:
			if(acq.header.sensors & MAG_MASK)
				return true;
			break;
		case TEMPERATURE:
			if(acq.header.sensors & BAR_MASK)
				return true;
			break;
		case PRESSURE:
			if(acq.header.sensors & BAR_MASK)
				return true;
			break;
		case ALTITUDE:
			if(acq.header.sensors & BAR_MASK)
				return true;
			break;
		case BATTERY_LVL:
			if(acq.header.sensors & BAT_MASK)
				return true;
			break;
		case DATE:
			if(acq.header.sensors & GPS_MASK)
				return true;
			break;
		case TIME:
			if(acq.header.sensors & GPS_MASK)
				return true;
			break;
		case SATELLITES:
			if(acq.header.sensors & GPS_MASK)
				return true;
			break;
		case LATITUDE:
			if(acq.header.sensors & GPS_MASK)
				return true;
			break;
		case LONGITUDE:
			if(acq.header.sensors & GPS_MASK)
				return true;
			break;
		case ALT_GPS:
			if(acq.header.sensors & GPS_MASK)
				return true;
			break;
		case SPEED:
			if(acq.header.sensors & GPS_MASK)
				return true;
			break;
		case ANGLE:
			if(acq.header.sensors & GPS_MASK)
				return true;
			break;
		default:
			return false;
	}
	return false;
}

float Acquisition::readValue(int _plot_object)
{
	//return 10.0 * sin((double)SDL_GetTicks()/1000);
	switch(_plot_object)
	{
		case X_ACCELERATION:
			return acq.acc.ax;
		case Y_ACCELERATION:
			return acq.acc.ay;
		case Z_ACCELERATION:
			return acq.acc.az;
		case X_ROTATION:
			return acq.gyr.gx;
		case Y_ROTATION:
			return acq.gyr.gy;
		case Z_ROTATION:
			return acq.gyr.gz;
		case X_MAGNETIC_FLUX:
			return acq.mag.hx;
		case Y_MAGNETIC_FLUX:
			return acq.mag.hy;
		case Z_MAGNETIC_FLUX:
			return acq.mag.hz;
		case TEMPERATURE:
			return acq.bar.temperature;
		case PRESSURE:
			return acq.bar.pressure;
		case ALTITUDE:
			return acq.bar.altitude;
		case BATTERY_LVL:
			return acq.voltage;
		case SATELLITES:
			return (float)acq.gps.nb_sat;
		case ALT_GPS:
			return acq.gps.alt_gps;
		case SPEED:
			return acq.gps.speed;
		case ANGLE:
			return acq.gps.angle;
		case LATITUDE:
			return acq.gps.latitude;
		case LONGITUDE:
			return acq.gps.longitude;
		default:
			return 0;
	}
}

void Acquisition::readDate(char* _date)
{
	sprintf(_date, "%02u/%02u/20%02u", acq.gps.day, acq.gps.month, acq.gps.year);
}

void Acquisition::readTime(char* _time)
{
	sprintf(_time, "%02u:%02u.%02u", acq.gps.hour, acq.gps.min, acq.gps.sec);
}

void Acquisition::readLatitude(char* _latitude)
{
	sprintf(_latitude, "% -6.2f  %c", acq.gps.latitude, acq.gps.lat);
	_latitude[strlen(_latitude) - 3] = '°';
}

void Acquisition::readLongitude(char* _longitude)
{
	sprintf(_longitude, "% -6.2f  %c", acq.gps.longitude, acq.gps.lon);
	_longitude[strlen(_longitude) - 3] = '°';
}

Sniffer::Sniffer()
{
	char arg1[16];
	char arg2[16];
	int p_desc1[2];
	int p_desc2[2];

	strcpy(file_name, "/dev/rfcomm0");

	pipe(p_desc1);
	pipe(p_desc2);
	p_mosi = p_desc1[1];
	p_miso = p_desc2[0];

	sprintf(arg1, "%d", p_desc1[0]);
	sprintf(arg2, "%d", p_desc2[1]);
	
	pid = fork();
	if(pid == 0)
	{
		execl("Sniffer", "Sniffer", arg1, arg2, NULL);
		exit(0);
	}
	connection_status = CONNECTED;
}

Sniffer::~Sniffer()
{
	write(p_mosi, "QUIT", 4);
	close(p_miso);
	close(p_mosi);
	wait(NULL);
}

void Sniffer::configureSensors(unsigned char _sensors)
{
	char buf[BUF_SIZE];
	strncpy(buf, "SENS", CMD_SIZE);
	write(p_mosi, buf, CMD_SIZE);
	buf[0] = _sensors;
	write(p_mosi, buf, 1);
}

void Sniffer::connectBluetooth()
{
	char buf[BUF_SIZE];
	strncpy(buf, "CONN", CMD_SIZE);
	write(p_mosi, buf, CMD_SIZE);
}

bool Sniffer::updateConnectionStatus()
{
	char buf[BUF_SIZE];
	strncpy(buf, "STAT", CMD_SIZE);
	write(p_mosi, buf, CMD_SIZE);
	read(p_miso, buf, 1);
	if(buf[0] == 'C')
		connection_status = CONNECTED;
	else
		connection_status = DISCONNECTED;
	return connection_status;
}

void Sniffer::enterBurstMode()
{
	char buf[BUF_SIZE];
	strncpy(buf, "BURS", CMD_SIZE);
	write(p_mosi, buf, CMD_SIZE);
}

Acquisition Sniffer::getLastAcq()
{
	ACQ_s acq;
	if(connection_status == CONNECTED)
	{
		write(p_mosi, "LAST", 4);
		read(p_miso, (char*)&acq, sizeof(ACQ_s));
	}
	else
	{
		acq.header.sensors = 0x00;
	}
	Acquisition acquisition(acq);

	return acquisition;
}






int set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                //error_message ("error %d from tcgetattr", errno);
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                //error_message ("error %d from tcsetattr", errno);
                return -1;
        }
        return 0;
}

void set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                //error_message ("error %d from tggetattr", errno);
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        	return;
                //error_message ("error %d setting term attributes", errno);
}

bool connect(int* fd, char* file_name)
{
	char buf[BUF_SIZE];

	*fd = open (file_name, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0)
	{
		printf("CANNOT CONNECT TO DEVICE\n");
    	return DISCONNECTED;
	}

	printf("SUCCESS: CONNECTED TO DEVICE\n");

	set_interface_attribs (*fd, B115200, 0);	// set speed to 115,200 bps, 8n1 (no parity)
	set_blocking (*fd, 0);                		// set no blocking

	while(read(*fd, buf, BUF_SIZE) > 0)
		;

	//SDL_Delay(1000);

	return CONNECTED;
}
