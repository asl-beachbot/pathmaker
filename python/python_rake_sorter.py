f_rake = open("Hai/Hairake.txt")
f_orig = open("Hai/hai_pfad.txt")
f_path = open("Hai/Haipath.txt")


thresh = 0.2
import plot_path
from math import sqrt
class Point():
	def __init__(self, x, y, rake = None):
		self.x = x
		self.y = y
		self.rake = rake
		return
		if rake:
			self.rake = int(rake)
		else:
			self.rake = None
	def __repr__(self):
		return "x %f y %f" % (self.x, self.y)



def sq_dist(p1, p2):
	return sqrt((p1.x - p2.x) ** 2 + (p1.y - p2.y) ** 2)

def print_path(p):
	for i in p:
		print("{0} {1} {2}".format(i.x, i.y, i.rake))

def create_path(f):
	res = list()
	for l in f.readlines():
		k = l.split(" ")
		res.append(Point(float(k[0]), float(k[1]), float(k[2]) if len(k) > 2 else None))
	return res

def main():
	rake = create_path(f_rake)
	orig_path = create_path(f_orig)
	path = create_path(f_path)

	res_path = list()

	for i in xrange(len(path)):
		path[i].rake = rake[i].rake

	# get points where rake is c2hanging
	pl_rake_change = list()
	pl_rake_change.append(orig_path[0])
	prev_rake = orig_path[0].rake
	for it in orig_path:
		if it.rake != prev_rake:
			pl_rake_change.append(it)
			prev_rake = it.rake

	prev_dist = sq_dist(pl_rake_change[1], path[0])
	index_list = list()
	for e in pl_rake_change:
		count = 0
		for i in pl_rake_change:
			d = sq_dist(e, i)
			if(d < thresh):
				count += 1
				if(d == 0):
					n = count
					print(n)
					index_list.append(n)
	j = 1
	
	prev_rake = rake[0].rake
	i = 1
	for it in rake:
		if it.rake != prev_rake:
			prev_rake = it.rake 
			i += 1

	for i in pl_rake_change:
		print "%r %d\n" % (i, i.rake)

	i = 0
	while(j < len(pl_rake_change)):
		prev_dist = 100000000
		num_maxima = index_list[j]
		if(num_maxima > 1):
			prev_dist = 0
			while True:
				dist = sq_dist(pl_rake_change[j], path[i])
				if(dist > prev_dist):
					prev_dist = dist
					res_path.append(Point(rake[i].x, rake[i].y, pl_rake_change[j-1].rake))
					i = i + 1
				else:
					print("max", path[i])
					break
				# search until descending again
		prev_dist = 1000000
		while True:
			# print "\n %d" % i
			# print "prev_dist: %f, sq_dist: %f" % (prev_dist, sq_dist(pl_rake_change[j], path[i]))
			# print "%r , %r " % (pl_rake_change[j], path[i])
			dist = sq_dist(pl_rake_change[j], path[i])
			if (prev_dist > dist):
				prev_dist = dist
				res_path.append(Point(rake[i].x, rake[i].y, pl_rake_change[j-1].rake))
				i=i+1
			else:
				break
		# if i > 30:
		# 	outfile = open("or.txt", "w")
		# 	for i in res_path: 
		# 		outfile.write("{0} {1} {2}\n".format(i.x, i.y, i.rake))
		# 	return
		j += 1
	for i in xrange(i, len(path)):
		res_path.append(Point(rake[i].x, rake[i].y, pl_rake_change[j - 1].rake))

	outfile = open("or.txt", "w")
	for i in res_path: 
		outfile.write("{0} {1} {2}\n".format(i.x, i.y, i.rake))
	outfile.flush()
	print "running plt path"
	plot_path.do_plot("or.txt")
	# print_path(res_path)

if __name__ == '__main__':
	main()
