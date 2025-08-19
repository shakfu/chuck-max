Webcam webcam;

T.assert(webcam.deviceID() == 0, "default device ID is 0");
T.assert(webcam.capture(), "default webcam.capture() returns true");
T.assert(webcam.freeze() == false, "default webcam.freeze() returns false");
webcam.capture(false);
T.assert(webcam.capture() == false, "webcam.capture() can be set to false");
webcam.capture(true);
T.assert(webcam.capture() == true, "webcam.capture() can be set to true");
webcam.freeze(true);
T.assert(webcam.freeze() == true, "webcam.freeze() can be set to true");
webcam.freeze(false);
T.assert(webcam.freeze() == false, "webcam.freeze() can be set to false");
