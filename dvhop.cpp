#include <iostream>
#include <string>
#include <fstream>
#include <utility>
#include <cmath>

std::pair<double, double> nodeCoordinate[3];

void setCoordinates(double, double, double, double, double, double);
void trilateralMethod();

double avgHopSize = 100;

int main(int argc, char *argv[]){
        // Set anchor node coordinates x1, x2, x3, y1, y2, y3
        // Coordinates taken from github
        setCoordinates(123.42, 4534.452, 6663.42, 566.646, 123.42, 9873.54);
        trilateralMethod();
        for (int i = 0; i <= 3; i++){
                std::cout << "<" << nodeCoordinate[i].first << ", " << nodeCoordinate[i].second << ">\n";
        }
}

// Sets coordinates for each node
void setCoordinates(double x1, double y1, double x2, double y2, double x3, double y3) {
        nodeCoordinate[0] = std::make_pair(x1, y1);
        nodeCoordinate[1] = std::make_pair(x2, y2);
        nodeCoordinate[2] = std::make_pair(x3, y3);
}

// When unknown nodes get three or more distance information from beacon nodes, they
// can use trilateral method to estimate their coordinates.
void trilateralMethod() {
        // Unit vectors from (x1, y1) to (x2, y2)
        double exx = (nodeCoordinate[1].first - nodeCoordinate[0].first)/(sqrt(pow((nodeCoordinate[1].first 
			- nodeCoordinate[0].first), 2) + pow((nodeCoordinate[1].second - nodeCoordinate[0].second), 2)));
        double exy = (nodeCoordinate[1].second - nodeCoordinate[0].second)/(sqrt(pow((nodeCoordinate[1].first 
			- nodeCoordinate[0].first), 2) + pow((nodeCoordinate[1].second - nodeCoordinate[0].second), 2)));
        // Vector magnitudes x direction
        double ix = exx * (nodeCoordinate[2].first - nodeCoordinate[0].first);
        double iy = exy * (nodeCoordinate[2].second - nodeCoordinate[0].second);
        double i = ix + iy;
        // Unit vectors from (x1, y1) to (x3, y3)
        double eyx = (nodeCoordinate[2].first - nodeCoordinate[0].first- i*exx)/(sqrt(pow((nodeCoordinate[2].first 
			- nodeCoordinate[0].first- i*exx), 2) + pow((nodeCoordinate[2].second - nodeCoordinate[0].second - i*exy), 2)));
        double eyy = (nodeCoordinate[2].second - nodeCoordinate[0].second - i*exy)/(sqrt(pow((nodeCoordinate[2].first 
			- nodeCoordinate[0].first- i*exx), 2) + pow((nodeCoordinate[2].second - nodeCoordinate[0].second - i*exy), 2)));
        // Vector magnitudes y direction
        double jx = eyx * (nodeCoordinate[2].first - nodeCoordinate[0].first);
        double jy = eyy * (nodeCoordinate[2].second - nodeCoordinate[0].second);
        double j = jx + jy;

        std::pair <double, double> coordinates;
        double x = pow(sqrt(pow((nodeCoordinate[1].first - nodeCoordinate[0].first), 2) + pow((nodeCoordinate[1].second 
			- nodeCoordinate[0].second), 2)), 2) / (2 * sqrt(pow((nodeCoordinate[1].first - nodeCoordinate[0].first), 2) 
			+ pow((nodeCoordinate[1].second - nodeCoordinate[0].second), 2)));
        double y = (pow(i, 2) + pow(j, 2)) / (2 * j) -i * (x/j);

        double xCoordinate = nodeCoordinate[0].first + x * exx + y * eyx;
        double yCoordinate = nodeCoordinate[0].second + x * exy + y * eyy;

        nodeCoordinate[3] = std::make_pair(xCoordinate, yCoordinate);
}
