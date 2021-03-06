import numpy as np
import matplotlib.pyplot as plt # google this for more info
import csv
#make sure the csv file has all columns filled out on last row
#this will plot all 6 axes's on one graph
#put this file in same file as the signal; cd to this directory and then "python plot_walk.py" on terminal
#change variable "signal_fname" in line 25 to choose which signal to plot 
#can overlap two plots with this script to discern features


def read_file(fname):
	data = []
	with open(fname, "rU") as ifile:
		reader = csv.reader(ifile)
		for row in reader:
			try:
				data.append([float(i) for i in row])
			# this will discard any rows that can't 
			# be converted to floats
			except ValueError:
				pass
				
	data = np.array(data)	
	return data
	
def main(): 
	signal_fname 	= "turn_right_thrice_norm.csv"

    # peaks_fname 	= "acceleration_output.csv"

	# read data from file
	signal 	= read_file(signal_fname)

    # peaks 	= read_file(peaks_fname)

	# associate signals to names from signal matrix change below
	t_s = signal[:, 0]
	t_g = signal[:, 1]
	x = signal[:, 2]
	y = signal[:, 3]
	z = signal[:, 4]
	g_x = signal[:, 5]
	g_y = signal[:, 6]
	g_z = signal[:, 7]
	


	fig = plt.figure()
	#ax1 = fig.add_subplot(111)
	#ax1.set_title("Accel_X")
	#ax1.plot(t_s, x, linewidth=0.8)
	ax2 = fig.add_subplot(111)
	ax2.set_title("Accel_Y")
	ax2.plot(t_s, y, linewidth=0.8)
	ax2.plot(t_g, g_y, linewidth=0.8)

	plt.show()


	
if __name__ == "__main__":
	main()
