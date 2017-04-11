import numpy as np
import matplotlib.pyplot as plt # google this for more info
import csv


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
	signal_fname 	= "circle_right.csv"

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
	ax1 = fig.add_subplot(231)
	#fig, ax = plt.subplots()
	ax1.set_title("Accel_X")
	ax1.plot(t_s, x, linewidth=0.8)
	ax2 = fig.add_subplot(232)
	ax2.set_title("Accel_Y")
	ax2.plot(t_s, y, linewidth=0.8)
	ax3 = fig.add_subplot(233)
	ax3.set_title("Accel_Z")	
	ax3.plot(t_s, z, linewidth=0.8)
	ax4 = fig.add_subplot(234)
	ax4.set_title("Gyro_X")
	ax4.plot(t_g, g_x, linewidth=0.8)
	ax5 = fig.add_subplot(235)
	ax5.set_title("Gyro_Y")
	ax5.plot(t_g, g_y, linewidth=0.8)
	ax6 = fig.add_subplot(236)
	ax6.plot(t_g, g_z, linewidth=0.8)
	ax6.set_title("Gyro_Z")

	plt.show()


	
if __name__ == "__main__":
	main()
