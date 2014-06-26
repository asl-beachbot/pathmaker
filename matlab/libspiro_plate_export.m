clear
if exist('OCTAVE_VERSION')
	more off
end
p1 = [200 200];

% v1 = [0.75, 0.25];
v1 = [-1, 0.1]

p2 = [250, 200];
v2 = [1, 0.1];

v1n = v1 / norm(v1);
v2n = v2 / norm(v2);
r = 80;

p1_tangente = p1 + v1n * 10;
p2_tangente = p2 + v2n * 10;

dist_points = norm(p1 - p2)

conn_line = p1 - p2;
no_curve_points = false
if norm(conn_line) ~= 0 
	conn_line_n = conn_line/norm(conn_line)
	conn_angle_1 = pi - acos(dot(v1n, conn_line_n))
	conn_angle_2 = acos(dot(v2n, conn_line_n))
	vec_angle = acos(dot(v1n, v2n))
	side_1 = -1 * v1n(2) * conn_line_n(1) + v1n(1) * conn_line_n(2)
	side_2 = -1 * v2n(2) * conn_line_n(1) + v2n(1) * conn_line_n(2)
else
	conn_angle_1 = 0
	conn_angle_2 = 0
end


% if conn_angle_1 < pi/5 && conn_angle_2 < pi/5 && vec_angle < pi/5
% 	no_curve_points = true
% end


% ui = intersect_1;
str = strcat(
	sprintf('(plate\n'),
	sprintf('\t(v %f %f)\n', p1(1), p1(2)),
	sprintf('\t(] %f %f)\n', p1_tangente(1), p1_tangente(2))
);

% if angle between the two lines > Pi/2 then it should take the normals to the inside

ang = acos(dot(v1n, v2n))
r90 = [cos(pi/2), -sin(pi/2); sin(pi/2), cos(pi/2)];
rm90 = [cos(-pi/2), -sin(-pi/2); sin(-pi/2), cos(-pi/2)];
%if(conn_angle_1 > pi/2)
%	conn_angle_1 = - pi + conn_angle_1
%end
%if(conn_angle_2 > pi/2)
%	conn_angle_2 = pi + conn_angle_2
%end
% conn_angle_2 =  - pi/2 - conn_angle_2
% if conn_angle_1 < 0.1
%   conn_angle_1 = pi/2
% end
% if conn_angle_2 < 0.1
%   conn_angle_2 = pi/2
% end
% conn_angle_2 = -conn_angle_2
r1 = -sign(side_1) * [cos(conn_angle_1/2), -sin(conn_angle_1/2); sin(conn_angle_1/2), cos(conn_angle_1/2)];
r2 = -sign(side_2) * [cos(conn_angle_2/2), -sin(conn_angle_2/2); sin(conn_angle_2/2), cos(conn_angle_2/2)];
if ~no_curve_points
	% if ang > pi / 2
	% 	u1 = p1 + v1n * 10 + (r90 * v1n' * r)';
	% 	u2 = p2 + v2n * 10 + (rm90 * v2n' * r)';
	% else
	% end
	% u1 = p1 + v1n * r;
	% u2 = p2 + v2n * r;
	% if(norm(p1-p2) > 2*r)
	u1 = p1_tangente + ( v1n')' * r
	u2 = p2_tangente + ( v2n')' * r
	if sign(side_1) == sign(side_2)
		u1 = p1_tangente + (r1 * v1n')' * r
		u2 = p2_tangente + (r2 * -v2n')' * r
	end

	% else
		% u1 = p1_tangente +v1n * r
		% u2 = p2_tangente + v2n * r
	% end
	% distance between u1 and u2?
	disp('asd')
	du1u2 = norm(u1 - u2)
	if(du1u2 < r)
		% edge case!
		% push d1 and d2 away from each other
		disp('pushing away')

		if(conn_angle_1 < pi/2 && conn_angle_2 < pi/2 && dist_points < 0.5 * r)
			disp("case1")
			dr1 = [cos(0.01), -sin(0.01); sin(0.01), cos(0.01)];
			dr2 = [cos(-0.01), -sin(-0.01); sin(-0.01), cos(-0.01)];
		elseif(conn_angle_1 > pi/2 && conn_angle_2 < pi/2)
		  	disp("case2")
			dr1 = [cos(0.01), -sin(0.01); sin(0.01), cos(0.01)];
			dr2 = [cos(-0.01), -sin(-0.01); sin(-0.01), cos(-0.01)];
		elseif (conn_angle_1 > pi/2 && conn_angle_2 < pi/2)
		  disp("case3")
		  	dr1 = [cos(-0.01), -sin(-0.01); sin(-0.01), cos(-0.01)];
			dr2 = [cos(0.01), -sin(0.01); sin(0.01), cos(0.01)];
		elseif (conn_angle_1 > pi/2 && conn_angle_2 > pi/2)
			disp("case4")
			if(sign(side_1) < 0)
				dr1 = [cos(-0.01), -sin(-0.01); sin(-0.01), cos(-0.01)];
				dr2 = [cos(0.01), -sin(0.01); sin(0.01), cos(0.01)];
			else
				dr1 =  [cos(0.01), -sin(0.01); sin(0.01), cos(0.01)];
				dr2 =  [cos(-0.01), -sin(-0.01); sin(-0.01), cos(-0.01)];
			end
		end
		u1 = p1_tangente + (r1 * v1n')' * r
		u2 = p2_tangente + (r2 * -v2n')' * r
		n = 1
		d = du1u2
		while(d < r)
				u1 = p1_tangente + (dr1^n * r1 * v1n')' * r;
				u2 = p2_tangente + (dr2^n * r2 * -v2n')' * r;
				n = n + 1;
				d = norm(u1 - u2)
		end


		% conn_line = u1 - u2
		% if(norm(conn_line) == 0)
		% 	conn_line_n  = r90 * v1n;
		% 	% conn_line_n = conn_line / sqrt(norm(conn_line))
		% else
		% 	conn_line_n = conn_line / norm(conn_line)
		% end
		% mid_point = u1 + (u1 - u2) / 2
		% u1 = mid_point + conn_line_n * r/2
		% u2 = mid_point - conn_line_n * r/2
		% str = strcat(
		% 	str,
		% 	sprintf('\t(o %f %f)\n', u1(1), u1(2))
		% );
	% else
	end
	% if(vec_angle > 3*pi/5 && du1u2 > r)
	% 	disp('pushing away2')
	% 	conn_line = u1 - u2
	% 	conn_line_n = conn_line / norm(conn_line)
	% 	mid_point = u1 + (u2 - u1) / 2
	% 	u1 = mid_point + conn_line_n * r / 2
	% 	u2 = mid_point - conn_line_n * r / 2
	% 	% if(norm(u1 - p1) < r)
	% 	% 	% also push outside
	% 	% 	conn_line = u1 - p1;
	% 	% 	conn_line_n = conn_line / norm(conn_line)
	% 	% 	u1 = u1 + conn_line_n * (r/(norm(u1 - p1)))
	% 	% end
	% 	% if(norm(u2 - p1) < r)
	% 	% 	% also push outside
	% 	% 	conn_line = u2 - p1;
	% 	% 	conn_line_n = conn_line / norm(conn_line)
	% 	% 	u2 = u2 + conn_line_n * (r/(norm(u2 - p1)))
	% 	% end
	% end

	% % case that it stretches very far
	% if vec_angle > 3/4 * pi
	% 	% add another point!
	% end

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
)

	% sprintf('\t(o %d %d)\n', 300, 400),

filename = 'plate';
fid = fopen (filename, 'w');
fputs (fid, str);
fclose (fid);
system('./ppedit_nonverbose plate &> /dev/null', 0, 'async');