#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstdlib>
#include <string>




#include <thread>
#include <atomic>
#include <chrono>
#include <cstring>
#include <stdio.h>
#include <ctime>
#include <unistd.h>
#include <MQTTClient.h>
#include <fstream>
#include <thread>

#include <mysql/mysql.h>
#include "stdlib.h"
#include "unistd.h"

#include <signal.h>

#define ADDRESS     "localhost"
#define CLIENTID    "MQTT_Server"

using namespace cv;
using namespace std;

int counter = 0;

class FFError
{
public:
    std::string    Label;

    FFError( ) { Label = (char *)"Generic Error"; }
    FFError( char *message ) { Label = message; }
    ~FFError() { }
    inline const char*   GetMessage  ( void )   { return Label.c_str(); }
};

MYSQL      *MySQLConRet;
MYSQL      *MySQLConnection = NULL;

Mat frame;
Mat frame1;

int detectAndDraw( Mat& img, CascadeClassifier& cascade,
                    CascadeClassifier& nestedCascade,
                    double scale, bool tryflip );

void handle_signal(int sig) {
    char buffer[50];
    sprintf(buffer, "/home/sina/Desktop/Project_98101685/FaceRecognition/data/%ld.jpg", time(0));
    imwrite(buffer, frame1);
}

void publish(MQTTClient client, const char* topic, char* payload) {
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    pubmsg.payload = payload;
    pubmsg.payloadlen = strlen((char*)pubmsg.payload);
    pubmsg.qos = 2;
    pubmsg.retained = 0;
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    MQTTClient_waitForCompletion(client, token, 1000L);
    printf("%s\n", payload);
}

int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    char* payload = (char*)message->payload;
    counter++;
    printf("%d)Received operation %s\n", counter, payload);
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

CascadeClassifier cascade, nestedCascade;
double scale = 2;
bool tryflip = false;

// buffers
char path[50];
char req[100];
char tm_buffer[30];
char buffer[200];

void mysql_write(int number_of_faces, char* dt, const char* table) {
    sprintf(req, "INSERT INTO %s (time) VALUES ('%d', '%s')",table, number_of_faces, dt);
    if (mysql_query(MySQLConnection, req) ) {
        printf("Error %u: %s\n", mysql_errno(MySQLConnection), mysql_error(MySQLConnection));
		exit(1);
    }
}

void mysql_write(float intensity, char* dt, const char* table) {
    sprintf(req, "INSERT INTO %s ( time) VALUES ('%f', '%s')",table, intensity, dt);
    if (mysql_query(MySQLConnection, req) ) {
        printf("Error %u: %s\n", mysql_errno(MySQLConnection), mysql_error(MySQLConnection));
		exit(1);
    }
}





int main( )
{
    signal(SIGUSR1, handle_signal);

	// MQTT initialize
	MQTTClient client;
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.username = "sinazeraatkar";
    conn_opts.password = "sina4432";

    MQTTClient_setCallbacks(client, NULL, NULL, on_message, NULL);

    int rc;
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    }

    // Connect to the database
    string hostName = "localhost";
    string userId   = "sinalolo";
    string password = "sina4432";
    MySQLConnection = mysql_init( NULL );
    try {
        MySQLConRet = mysql_real_connect( MySQLConnection,
                                          hostName.c_str(), 
                                          userId.c_str(), 
                                          password.c_str(), 
                                          "recognition_database",
                                          0, 
                                          NULL,
                                          0 );
        if ( MySQLConRet == NULL )
            throw FFError( (char*) mysql_error(MySQLConnection) );
        printf("MySQL Connection Info: %s \n", mysql_get_host_info(MySQLConnection));
        printf("MySQL Client Info: %s \n", mysql_get_client_info());
        printf("MySQL Server Info: %s \n", mysql_get_server_info(MySQLConnection));
    }
    catch ( FFError e ) {
        printf("%s\n",e.Label.c_str());
        return 1;
    }

    cv::VideoCapture camera(0);
    if (!camera.isOpened()) {
        std::cerr << "ERROR: Could not open camera" << std::endl;
        return 1;
    }
    if(!cascade.load("/home/sina/Desktop/Project_98101685/FaceRecognition/haarcascade_frontalface_alt.xml"))
    {
        cout << "--(!)Error loading face cascade\n";
        return -1;
    }
    
	int last_number_of_faces = 0;
    int last_ble_status = -1;
	for(;;) {
		camera >> frame;
		if( frame.empty() )
			break;
		

		int number_of_faces = detectAndDraw( frame, cascade, nestedCascade, scale, tryflip );
		if(number_of_faces != last_number_of_faces){
            time_t now = time(0);
            tm *ltm = localtime(&now);
            sprintf(tm_buffer, "%d:%d:%d", ltm->tm_hour, ltm->tm_min, ltm->tm_sec);

        	sprintf(buffer, "%s | number of faces = %d", tm_buffer, number_of_faces);
        	publish(client, "data/faces", buffer);
            mysql_write(number_of_faces, "faces", "faces");
		}
       
        last_number_of_faces = number_of_faces;
		char c = (char)waitKey(10);
		if( c == 27 || c == 'q' || c == 'Q' )
			break;
	}

    // close MYSQL
	mysql_close(MySQLConnection);

    // close MQTT
	MQTTClient_disconnect(client, 1000);
    MQTTClient_destroy(&client);
}


int detectAndDraw( Mat& img, CascadeClassifier& cascade,
                    CascadeClassifier& nestedCascade,
                    double scale, bool tryflip )
{
    double t = 0;
    vector<Rect> faces, faces2;
    const static Scalar colors[] =
    {
        Scalar(255,0,0),
        Scalar(255,128,0),
        Scalar(255,255,0),
        Scalar(0,255,0),
        Scalar(0,128,255),
        Scalar(0,255,255),
        Scalar(0,0,255),
        Scalar(255,0,255)
    };
    Mat gray, smallImg;

    cvtColor( img, gray, COLOR_BGR2GRAY );
    double fx = 1 / scale;
    resize( gray, smallImg, Size(), fx, fx, INTER_LINEAR_EXACT );
    equalizeHist( smallImg, smallImg );

    t = (double)getTickCount();
    cascade.detectMultiScale( smallImg, faces,
        1.1, 2, 0
        |CASCADE_SCALE_IMAGE,
        Size(30, 30) );
    if( tryflip )
    {
        flip(smallImg, smallImg, 1);
        cascade.detectMultiScale( smallImg, faces2,
                                 1.1, 2, 0
                                 |CASCADE_SCALE_IMAGE,
                                 Size(30, 30) );
        for( vector<Rect>::const_iterator r = faces2.begin(); r != faces2.end(); ++r )
        {
            faces.push_back(Rect(smallImg.cols - r->x - r->width, r->y, r->width, r->height));
        }
    }
    t = (double)getTickCount() - t;
    for ( size_t i = 0; i < faces.size(); i++ )
    {
        Rect r = faces[i];
        Mat smallImgROI;
        vector<Rect> nestedObjects;
        Point center;
        Scalar color = colors[i%8];
        int radius;

        double aspect_ratio = (double)r.width/r.height;
        if( 0.75 < aspect_ratio && aspect_ratio < 1.3 )
        {
            center.x = cvRound((r.x + r.width*0.5)*scale);
            center.y = cvRound((r.y + r.height*0.5)*scale);
            radius = cvRound((r.width + r.height)*0.25*scale);
            circle( img, center, radius, color, 3, 8, 0 );
        }
        else
            rectangle( img, Point(cvRound(r.x*scale), cvRound(r.y*scale)),
                       Point(cvRound((r.x + r.width-1)*scale), cvRound((r.y + r.height-1)*scale)),
                       color, 3, 8, 0);
        if( nestedCascade.empty() )
            continue;
        smallImgROI = smallImg( r );
        nestedCascade.detectMultiScale( smallImgROI, nestedObjects,
            1.1, 2, 0
            |CASCADE_SCALE_IMAGE,
            Size(30, 30) );
        for ( size_t j = 0; j < nestedObjects.size(); j++ )
        {
            Rect nr = nestedObjects[j];
            center.x = cvRound((r.x + nr.x + nr.width*0.5)*scale);
            center.y = cvRound((r.y + nr.y + nr.height*0.5)*scale);
            radius = cvRound((nr.width + nr.height)*0.25*scale);
            circle( img, center, radius, color, 3, 8, 0 );
        }
    }
    frame1 = img.clone();
    return faces.size();
}

