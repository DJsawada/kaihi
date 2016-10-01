/*!
  \example get_distance.cpp Obtains distance data
  \author Satofumi KAMIMURA

  $Id: get_distance.cpp,v c5747add6615 2015/05/07 03:18:34 alexandr $
*/
#include "Urg_driver.h"
#include "Connection_information.h"
#include "math_utilities.h"
#include <stdio.h>
#include <iostream>
using namespace qrk;
using namespace std;
namespace{
	void print_data(const Urg_driver& urg,
		const vector<long>& data, long time_stamp){
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
}
int main(int argc, char *argv[]){
	Connection_information information(argc, argv);
	// Connects to the sensor
	Urg_driver urg;
	if (!urg.open(information.device_or_ip_name(),
		information.baudrate_or_port_number(),
		information.connection_type())) {
			cout << "Urg_driver::open(): " << information.device_or_ip_name() << ": " << urg.what() << endl;
			return 1;
		}
	// Gets measurement data
	#if 1
	// Case where the measurement range (start/end steps) is defined
	urg.set_scanning_parameter(urg.deg2step(-90), urg.deg2step(+90), 0);
	#endif
	enum {
		Capture_times = 10
	};
	urg.start_measurement(Urg_driver::Distance, Urg_driver::Infinity_times, 0);
	for (int i = 0; i < Capture_times; ++i) {
		vector<long> data;
		long time_stamp = 0;
		if (!urg.get_distance(data, &time_stamp)) {
			cout << "Urg_driver::get_distance(): " << urg.what() << endl;
			return 1;
		}
		print_data(urg, data, time_stamp);
	}
	#if defined(URG_MSC)
	getchar();
	#endif
	return 0;
}
#include <3052.h>
unsigned int ad_dataud,ad_data_lr;
int i,j;
void main(void){
	ad_init();
	while(1){
		AD.ADCSR.BIT.ADST=1;//ad変換開始
		while(AD.DRCSR.BIT.ADF==0)//ad変換終了待ち
		ad_data_ud=AD.ADDRB;
		ad_data_ud=AD.ADDRC;
		ad_data_ud>>=8;//8bit右にシフト
		ad_data_lr>>=8;//8bit右にシフト
		if(ad_data_ud>0x8f){
			P1.DR.BYTE=0x01;//前進
			P3.DR.BYTE=0x0b;//前進
		}
		else if(ad_data_ud<0x70){
			P1.DR.BYTE=0x02;//後進
			P3.DR.BYTE=0x0e;//後進
		}
		else if(ad_data_ud>0x8f){
			P1.DR.BYTE=0x04;//右
			P3.DR.BYTE=0x0a;//右
		}
		else if(ad_data_ud<0x70){
			P1.DR.BYTE=0x08;//左
			P3.DR.BYTE=0x0f;//左
		}
		else{
			P1.DR.BYTE=0x10;//停止
			P3.DR.BYTE=0x00;//停止
		}
	}
}
void ad_init(void){
	P1.DDR.=0xff;
	P1.DR.BYTE=0;
	P3.DDR.=0xff;
	P3.DR.BYTE=0;

	AD.ADCSR.BIT.ADIE=0;
	AD.ADCSR.BIT.SCAN=1;
	AD.ADCSR.BIT.CKS=1;
	AD.ADCSR.BIT.CH=6;
}
