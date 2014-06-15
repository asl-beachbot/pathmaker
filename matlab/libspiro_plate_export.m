clear
p1 = [400, 200];

v1 = [0.75, 0.25];

p2 = [300, 200];
v2 = [-0.75, 0.25];

v1n = v1 / norm(v1);
v2n = v2 / norm(v2);
r = 200;

p1_tangente = p1 + v1n * 10;
p2_tangente = p2 + v2n * 10;

dist_points = norm(p1 - p2)

conn_line = p1 - p2;

conn_angle_1 = acos(dot(p1_tangente, conn_line) / norm(dot(p1_tangente, conn_line)))
conn_angle_2 = acos(dot(p2_tangente, conn_line) / norm(dot(p2_tangente, conn_line)))
vec_angle = acos(dot(v1n, v2n))
no_curve_points = false

if conn_angle_1 < pi/5 && conn_angle_2 < pi/5 && vec_angle < pi/5
	no_curve_points = true
end


% ui = intersect_1;
str = strcat(
	sprintf('(plate\n'),
	sprintf('\t(v %f %f)\n', p1(1), p1(2)),
	sprintf('\t(] %f %f)\n', p1_tangente(1), p1_tangente(2))
);

% if angle between the two lines > Pi/2 then it should take the normals to the inside

v1n
v2n

ang = acos(dot(v1n, v2n))
r90 = [cos(pi/2), -sin(pi/2); sin(pi/2), cos(pi/2)];
rm90 = [cos(-pi/2), -sin(-pi/2); sin(-pi/2), cos(-pi/2)];
if ~no_curve_points
	% if ang > pi / 2
	% 	u1 = p1 + v1n * 10 + (r90 * v1n' * r)';
	% 	u2 = p2 + v2n * 10 + (rm90 * v2n' * r)';
	% else
	% end
	u1 = p1 + v1n * r
	u2 = p2 + v2n * r

	% distance between u1 and u2?
	disp('asd')
	du1u2 = norm(u1 - u2)
	if(du1u2 < r)
		% edge case!
		% push d1 and d2 away from each other
		conn_line = u1 - u2
		if(norm(conn_line) == 0)
			conn_line_n  = r90 * v1n;
			% conn_line_n = conn_line / sqrt(norm(conn_line))
		else
			conn_line_n = conn_line / sqrt(norm(conn_line))
		end
		mid_point = u1 + (u1 - u2) / 2
		u1 = mid_point - conn_line_n * r/2
		u2 = mid_point + conn_line_n * r/2
		% str = strcat(
		% 	str,
		% 	sprintf('\t(o %f %f)\n', u1(1), u1(2))
		% );
	% else
	end
	if(vec_angle > 3*pi/5 && du1u2 > r)
		conn_line = u1 - u2
		conn_line_n = conn_line / norm(conn_line)
		mid_point = u1 + (u2 - u1) / 2
		u1 = mid_point + conn_line_n * r / 2
		u2 = mid_point - conn_line_n * r / 2
		% if(norm(u1 - p1) < r)
		% 	% also push outside
		% 	conn_line = u1 - p1;
		% 	conn_line_n = conn_line / norm(conn_line)
		% 	u1 = u1 + conn_line_n * (r/(norm(u1 - p1)))
		% end
		% if(norm(u2 - p1) < r)
		% 	% also push outside
		% 	conn_line = u2 - p1;
		% 	conn_line_n = conn_line / norm(conn_line)
		% 	u2 = u2 + conn_line_n * (r/(norm(u2 - p1)))
		% end
	end

	% case that it stretches very far
	if vec_angle > 3/4 * pi
		% add another point!
	end

	str = strcat(
		str,
		sprintf('\t(c %f %f)\n', u1(1), u1(2)),
		sprintf('\t(c %f %f)\n', u2(1), u2(2))
	);
end

str = strcat(
	str,
	sprintf('\t([ %f %f)\n', p2_tangente(1), p2_tangente(2)),
	sprintf('\t(v %f %f)\n', p2(1), p2(2)),
	sprintf(')\n')
);
disp(str)



	% sprintf('\t(o %d %d)\n', 300, 400),

filename = 'plate';
fid = fopen (filename, 'w');
fputs (fid, str);
fclose (fid);
system('./ppedit_nonverbose plate &> /dev/null', 0, 'async');