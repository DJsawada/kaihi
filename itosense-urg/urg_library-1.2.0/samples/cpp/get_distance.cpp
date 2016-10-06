/*!
  \example get_distance.cpp Obtains distance data
  \author Satofumi KAMIMURA

  $Id: get_distance.cpp,v c5747add6615 2015/05/07 03:18:34 alexandr $
*/

#include "Urg_driver.h"
#include "Connection_information.h"
#include "math_utilities.h"
#include <stdio.h>
#include <string>
#include <sysexits.h>
#include <math.h>
#include <iostream>
using namespace qrk;
using namespace std;
namespace{
	void print_data(const Urg_driver& urg, const vector<long>& data, long time_stamp){
		#if 1
		// Shows only the front step
		int front_index = urg.step2index(0);
		cout << data[front_index] << " [mm], (" << time_stamp << " [msec])" << endl;
		#else
		// Prints the X-Y coordinates for all the measurement points
		long min_distance = urg.min_distance();
		long max_distance = urg.max_distance();
		size_t data_n = data.size();
		for (size_t i = 0; i < data_n; ++i) {
			long l = data[i];
			if ((l <= min_distance) || (l >= max_distance)) {
				continue;
			}
			double radian = urg.index2rad(i);
			long x = static_cast<long>(l * cos(radian));
			long y = static_cast<long>(l * sin(radian));
			cout << "(" << x << ", " << y << ")" << endl;
		}
		cout << endl;
		#endif
	}
};

int sensor(Urg_driver &urg) {
	vector<long> data;
	long time_stamp = 0;
	if(!urg.get_distance(data, &time_stamp)){
		cout << "Urg_driver::get_distance(): " << urg.what() << endl;
		return 1;
	}
	//cout << data.size() << endl;
	short j;
	double angle = 180.0 / (data.size() - 1);
	for(j=0;j<data.size();j++){
		if(data[j] <= 20) continue;
		double theta,pi,x,y;
		pi=3.14159265359;
		double now = angle * j - 90;
		theta = now * pi / 180.0;
		//cout << "distance " << data[j] << "[mm] , angle " << now <<"[deg]" <<  endl;
		x=-data[j]*sin(theta);
		y=data[j]*cos(theta);
		//cout << "x" << x << ", y" << y << endl;
		if(x<150 && y<150 && x > -150 && y > -150){;
			return 1;
		}
	}
	return 0;	
}

int main(int argc, char *argv[]){
Connection_information information(argc, argv);
	// Connects to the sensor
	Urg_driver urg;
	if(!urg.open(information.device_or_ip_name(), information.baudrate_or_port_number(), information.connection_type())){
		cout << "Urg_driver::open(): " << information.device_or_ip_name() << ": " << urg.what() << endl;
		return 1;
	}
	// Gets measurement data
	#if 1
	// Case where the measurement range (start/end steps) is defined
	urg.set_scanning_parameter(urg.deg2step(-90), urg.deg2step(+90), 0);
	#endif
	enum{
		Capture_times = 721
	};
	urg.start_measurement(Urg_driver::Distance, Urg_driver::Infinity_times, 0);
	while(1){
		int return_value = sensor(urg);
		cout << return_value << endl;
        	//print_data(urg, data, time_stamp);
	}
	#if defined(URG_MSC)
	getchar();
	#endif
	return 0;
}
