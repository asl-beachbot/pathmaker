
p1 = [400, 200];

v1 = [-1, 0];

p2 = [300, 300];
v2 = [1, 0];

v1n = v1 / norm(v1);
v2n = v2 / norm(v2);
r = 200;

p11 = p1 + v1n * 10;
p22 = p2 - v2n * 10;

% ui = intersect_1;
str = strcat(
	sprintf('(plate\n'),
	sprintf('\t(v %f %f)\n', p1(1), p1(2)),
	sprintf('\t(] %f %f)\n', p11(1), p11(2))
);

% if angle between the two lines > Pi/2 then it should take the normals to the inside

v1n
v2n

ang = acos(dot(v1n, v2n))
r90 = [cos(pi/2), -sin(pi/2); sin(pi/2), cos(pi/2)];
rm90 = [cos(-pi/2), -sin(-pi/2); sin(-pi/2), cos(-pi/2)];
if ang > pi / 2
	u1 = p1 + v1n * 10 + (r90 * v1n' * r)';

	u2 = p2 + v2n * 10 + (rm90 * v2n' * r)';
		u2 = p2 + v2n * r;

else
	u1 = p1 + v1n * r;
	u2 = p2 + v2n * r;
end

% if(isreal(u1))
	str = strcat(
		str,
		sprintf('\t(c %f %f)\n', u1(1), u1(2)),
		sprintf('\t(c %f %f)\n', u2(1), u2(2))
	);
% end

str = strcat(
	str,
	sprintf('\t([ %f %f)\n', p2(1), p2(2)),
	sprintf('\t(v %f %f)\n', p22(1), p22(2)),
	sprintf(')\n')
);
disp(str)



	% sprintf('\t(o %d %d)\n', 300, 400),

filename = 'plate';
fid = fopen (filename, 'w');
fputs (fid, str);
fclose (fid);
[~] = system('./ppedit plate')
