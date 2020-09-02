.PHONY: a.out
a.out:
	g++ remote_controller.cpp -lwiringPi dht11.cpp relay.cpp -o a.out