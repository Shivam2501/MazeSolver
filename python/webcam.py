import numpy as np
import cv2


cap = cv2.VideoCapture(0)

while(True):
    # Capture frame-by-frame
    ret, frame = cap.read()

    # Our operations on the frame come here
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    edges = cv2.Canny(gray, 50, 100)

    # Display the resulting frame
    #cv2.imshow('frame',gray)
    cv2.imshow('frame', edges)
    #if cv2.waitKey(1) & 0xFF == ord('q'):
    #    break

    if cv2.waitKey(1) & 0xFF == ord('s'):
        cv2.imwrite('framesave.png', edges)
        break

# When everything done, release the capture
cap.release()
cv2.destroyAllWindows()