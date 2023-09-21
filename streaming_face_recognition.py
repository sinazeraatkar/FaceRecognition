import datetime
import sqlite3

import cv2

conn = sqlite3.connect('face_recognition.db')
c = conn.cursor()
c.execute('''CREATE TABLE IF NOT EXISTS face_records
             (timestamp TEXT, num_faces INTEGER)''')

num_faces_prev = 0

def process_frame(frame):
    global num_faces_prev

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    face_cascade = cv2.CascadeClassifier('haarcascade_frontalface_alt.xml')

    faces = face_cascade.detectMultiScale(gray, scaleFactor=1.3, minNeighbors=5)

    for i, (x, y, w, h) in enumerate(faces):
        cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
        cv2.putText(frame, str(i + 1), (x, y - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (0, 255, 0), 2)

    timestamp = str(datetime.datetime.now())
    num_faces = len(faces)

    if num_faces != num_faces_prev:
        c.execute("INSERT INTO face_records (timestamp, num_faces) VALUES (?, ?)", (timestamp, num_faces))
        conn.commit()

    num_faces_prev = num_faces

    cv2.putText(frame, timestamp, (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2)
    cv2.putText(frame, "Num Faces: " + str(num_faces), (10, 60), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2)

    cv2.imshow('Face Recognition', frame)

video_capture = cv2.VideoCapture(0)

while True:
    ret, frame = video_capture.read()

    process_frame(frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

video_capture.release()
cv2.destroyAllWindows()

conn.close()
