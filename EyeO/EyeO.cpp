#include <dlib/opencv.h>
#include <opencv2/highgui/highgui.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <wiringPi.h>

using namespace dlib;
using namespace std;

int main()
{
	int blinkCount = 0; 
   int initialIterations = 0;
	int warningCounter = 0;
   double leftAvg = 0;
   double rightAvg = 0;

	wiringPiSetup();
	pinMode(0, OUTPUT);
	digitalWrite(0,LOW);	
    try
    {
        cv::VideoCapture cap(0);
        if (!cap.isOpened())
        {
            cerr << "Unable to connect to camera" << endl;
            return 1;
        }
	
	cap.set(cv::CAP_PROP_FRAME_HEIGHT,90);

	cap.set(cv::CAP_PROP_FRAME_WIDTH,160);
        image_window win;

        // Load face detection and pose estimation models.
        frontal_face_detector detector = get_frontal_face_detector();
        shape_predictor pose_model;
        deserialize("shape_predictor_68_face_landmarks.dat") >> pose_model;

        // Grab and process frames until the main window is closed by the user.
        while (!win.is_closed())
        {
            
            // Grab a frame
            cv::Mat temp;
            cap >> temp;
            // Turn OpenCV's Mat into something dlib can deal with.  Note that this just
            // wraps the Mat object, it doesn't copy anything.  So cimg is only valid as
            // long as temp is valid.  Also don't do anything to temp that would cause it
            // to reallocate the memory which stores the image as that will make cimg
            // contain dangling pointers.  This basically means you shouldn't modify temp
            // while using cimg.
            cv_image<bgr_pixel> cimg(temp);

            // Detect faces
            std::vector<rectangle> faces = detector(cimg);
            // Find the pose of each face.
            std::vector<full_object_detection> shapes;
            for (unsigned long i = 0; i < faces.size(); ++i)
            {
                shapes.push_back(pose_model(cimg, faces[i]));
            }

            if (shapes.size() > 0)
            {
                full_object_detection shape = shapes[0];
                double leftMaxX = 0;
                double leftMaxY = 0;
                double leftMinX = 1000000;
                double leftMinY = 1000000;

                double rightMaxX = 0;
                double rightMaxY = 0;
                double rightMinX = 1000000;
                double rightMinY = 1000000;

                //left eye
                for (int i = 37; i < 42; i++)
                {
                    point p = shape.part(i);
                    if (p.x() > leftMaxX)
                    {
                        leftMaxX = p.x();
                    }

                    if (p.x() < leftMinX)
                    {
                        leftMinX = p.x();
                    }

                    if (p.y() > leftMaxY)
                    {
                        leftMaxY = p.y();
                    }

                    if (p.y() < leftMinY)
                    {
                        leftMinY = p.y();
                    }
                }

                //right eye
                for (int i = 43; i < 48; i++)
                {
                    point p = shape.part(i);
                    if (p.x() > rightMaxX)
                    {
                        rightMaxX = p.x();
                    }

                    if (p.x() < rightMinX)
                    {
                        rightMinX = p.x();
                    }

                    if (p.y() > rightMaxY)
                    {
                        rightMaxY = p.y();
                    }

                    if (p.y() < rightMinY)
                    {
                        rightMinY = p.y();
                    }
                }

                double leftRatio = (leftMaxX - leftMinX) / (leftMaxY - leftMinY);
                double rightRatio = (rightMaxX - rightMinX) / (rightMaxY - rightMinY);

                cout << leftRatio << "\n";
                cout << rightRatio << "\n";

                if (initialIterations < 10)
                {
                    cout << "init < 10\n";
                    cout.flush();
                    initialIterations++;
                    leftAvg += leftRatio;
                    rightAvg += rightRatio;
                }
                else if (initialIterations == 10)
                {
                    cout << "init == 10\n";
                    cout.flush();
                    initialIterations++;
                    leftAvg /= 10;
                    rightAvg /= 10;
                }
                else
                {
                    cout << "Left avg: " << leftAvg << "\n";
                    cout.flush();
                    if (leftRatio / leftAvg > 1.3 && rightRatio / rightAvg > 1.3)
                    {
                        cout << "WOOOOOOOOOO";
								if(blinkCount < 7) {
									blinkCount++;
								} else {
									digitalWrite(0, HIGH);
									warningCounter++;
									blinkCount = 0;
								}

                        //cout.flush();
							} else {
								blinkCount = 0;
							}
                }
					 if(warningCounter){
						if(warningCounter >= 15) {
							digitalWrite(0,LOW);
							warningCounter = 0;
						} else {
							warningCounter++;
						}
					 }
            }

            // Display it all on the screen
            win.clear_overlay();
            win.set_image(cimg);
            win.add_overlay(render_face_detections(shapes));
        }
    }
    catch (serialization_error &e)
    {
        cout << "You need dlib's default face landmarking model file to run this example." << endl;
        cout << "You can get it from the following URL: " << endl;
        cout << "   http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2" << endl;
        cout << endl
             << e.what() << endl;
    }
    catch (exception &e)
    {
        cout << e.what() << endl;
    }
}
