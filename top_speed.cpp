/*DISCLAIMER: The physics simulation here is rudimentary and should not be used to predict 0-60 times.*/


#include <iostream>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <algorithm>

using namespace std;


double get_gear_top_speed(double, double, double, double);
double get_tire_circumference(double, double, double);
double get_required_HP(double, double, double);
double get_time(double, double, double, double, double);
void print_chart(double, double, double, double, double);

int main(){

    double top_speed, last_gear_ratio, diff_gear_ratio, redline, tire_circ, wheel_size, tire_width, tire_height, drag_coefficient, frontal_area, mass;
    char r;

    
    cout<<"Input tire spec (e.g. 215 45r17): ";
    cin>>tire_width>>tire_height>>r>>wheel_size;
    tire_circ =get_tire_circumference(tire_width, tire_height, wheel_size);
    cout<<"Input the gear ratio of the highest gear: ";
    cin>>last_gear_ratio;
    cout<<"Input the gear ratio of your differential (final drive gear): ";
    cin>>diff_gear_ratio;
    cout<<"Input the redline (RPMs): ";
    cin>>redline;
    top_speed = get_gear_top_speed(last_gear_ratio, diff_gear_ratio, redline, tire_circ);
    cout<<"Gearing-limited top speed: "<<top_speed<<" MPH"<<endl;
    cout<<"Input car's drag coefficient: ";
    cin>>drag_coefficient;
    cout<<"Input car's frontal area (square meters): ";
    cin>>frontal_area;
    cout<<"Input car's mass (kg): ";
    cin>>mass;
    double HP = get_required_HP(top_speed, drag_coefficient, frontal_area);
    cout<<"Power required to gearing-limited "<<top_speed<<" MPH: "<<fixed<<HP<<endl;
    cout<<"Approximate ime to hit "<<setprecision(2)<<top_speed<<" at "<<HP<<" HP: ";
    cout<<get_time(top_speed, HP, mass, drag_coefficient, frontal_area)<<endl;
    print_chart(top_speed, HP, mass, drag_coefficient, frontal_area);

    double rounded_top_speed = round(top_speed/10.0) * 10;

    for(double speed = rounded_top_speed; speed>100; speed-=10){
        double new_HP = get_required_HP(speed, drag_coefficient, frontal_area);
        cout<<"\nPower required to hit "<<setprecision(2)<<speed<<" MPH: "<<fixed<<new_HP<<endl;
        cout<<"Time to hit "<<setprecision(2)<<speed<<" at "<<new_HP<<" HP: ";
        cout<<get_time(speed, new_HP, mass, drag_coefficient, frontal_area)<<endl;
    }
    

    // while(HP != 0){
    //     print_chart(top_speed, HP, mass, drag_coefficient, frontal_area);
    //     cin>>HP;
    // }

    return 0;
}

double get_tire_circumference(double tire_width, double tire_height, double wheel_size){
    cout<<"Width: "<<tire_width<<" Height: "<<tire_height<<" Wheel size: "<<wheel_size<<endl;
    double tire_circ = (3.14159265 * ((((tire_width * tire_height) / 2540) *2) + wheel_size));
    return tire_circ;

}
double get_gear_top_speed(double last_gear_ratio, double diff_gear_ratio, double redline, double tire_circ){
    double max_speed = 0;
    double a = redline * tire_circ;
    //cout<<a<<endl;
    //cout<<last_gear_ratio <<" x "<<diff_gear_ratio<<" = "<<last_gear_ratio*diff_gear_ratio<<endl;
    double b = last_gear_ratio * diff_gear_ratio;
    //cout<<b<<endl;
    max_speed = ((a) / (b) * 60) / 63360;
    //cout<< (redline * (tire_circ/3.1415)) / (diff_gear_ratio / last_gear_ratio); //* 336);
    double wheel_RPM = (redline / (diff_gear_ratio * last_gear_ratio)); 
    //cout<<"tire circumference is "<<tire_circ<<endl;
    double inches_per_min = wheel_RPM * tire_circ;
    return max_speed;
}

double get_required_HP(double top_speed, double drag_coefficient, double frontal_area){
    double required_HP = ((0.5 * drag_coefficient * 1.25 * frontal_area * pow((top_speed * 0.44704), 3))/745.7) + top_speed/10.5;
    //cout<<required_HP<<" watts = 0.5 x "<<drag_coefficient<<" x 1.25 x "<<frontal_area<<" x "<<pow(top_speed, 3)<<endl;
    return required_HP;
}

double get_time(double speed, double power, double mass, double drag_coefficient, double frontal_area){
    double speed_MpS = speed * 0.44704l;
    double drag = drag_coefficient * 1.25 * ((pow(speed_MpS, 2))/2) * frontal_area;
    double acceleration = (power * 745.7)/(mass * speed_MpS) - (drag/mass);
    double time_taken = 0.5 * pow(speed, 3) / (1.25 * frontal_area * drag_coefficient * (power * 745.7));

    double initial_velocity = 5;
    double final_velocity = 0;
    // for(int i = 0; i<300 && final_velocity <89.408; i++){
    //     drag = drag_coefficient * 1.25 * ((pow(initial_velocity, 2))/2) * frontal_area;
    //     acceleration = (power * 745.7)/(mass * initial_velocity) - (drag/mass);
    //     final_velocity = initial_velocity + acceleration;
    //     initial_velocity = final_velocity;
    //     cout<<final_velocity<<endl;
    // }
    return time_taken;
}




void print_chart(double speed, double power, double mass, double drag_coefficient, double frontal_area) {
    ofstream out;
    out.open("output.txt");
    double speed_MpS = speed * 0.44704;
    double drag, force, acceleration;
    double velocity = 0.0;
    double distance = 0.0;
    double dt = 0.001; // smaller time step for better precision

    cout <<endl<< "Simulating: " << endl;
    double time = 0.0;
    int tencount=0;

    do{
        drag = 0.5 * drag_coefficient * frontal_area * 1.225 * pow(velocity, 2);
        force = min(power * 745.7 / max(1.0, velocity), mass * 9.8) - drag; // capped force by max power

        acceleration = force / mass;
        velocity = velocity + acceleration * dt; // update velocity before computing distance
        distance += ((velocity) * dt);
        
        if ((tencount % 1000) == 0){ // adjusted based on the new `dt`
            cout << velocity * 2.23694 << " MPH at " << time << " seconds" << endl;
            out << velocity * 2.23694 << " MPH at " << time << " seconds" << endl;
        }
        
        time += dt;
        tencount += 1;

    } while(time < 60);

    cout << "Done" << endl;
    out.close();
}
