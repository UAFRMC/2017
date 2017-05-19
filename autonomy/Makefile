all: frontend backend vive kinect ipc

frontend: always
	cd frontend; make clean all

backend: always
	cd backend; make clean all

vive: always
	cd vive; make clean all

kinect: always
	cd kinect; make clean all

ipc: always
	cd ipc; make clean all

clean:
	for dir in frontend backend vive kinect ipc; do (cd $$dir; make clean); done

always:
	
