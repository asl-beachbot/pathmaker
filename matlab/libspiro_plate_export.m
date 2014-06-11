
p1 = [100, 100];

v1 = [0.1, 1];

p2 = [100, 100];
v2 = [0.1, -1];

v1n = v1 / norm(v1);
v2n = v2 / norm(v2);
r = 20;

A = p1; %# center of the first circle
B = p2; %# center of the second circle
a = r; %# radius of the SECOND circle
b = r; %# radius of the FIRST circle
c = norm(A - B); %# distance between circles

cosAlpha = (b^2+c^2-a^2)/(2*b*c);

u_AB = (B - A)/c; %# unit vector from first to second center
pu_AB = [u_AB(2), -u_AB(1)]; %# perpendicular vector to unit vector

%# use the cosine of alpha to calculate the length of the
%# vector along and perpendicular to AB that leads to the
%# intersection point
intersect_1 = A + u_AB * (b*cosAlpha) + pu_AB * (b*sqrt(1-cosAlpha^2));
intersect_2 = A + u_AB * (b*cosAlpha) - pu_AB * (b*sqrt(1-cosAlpha^2));

p11 = p1 + v1n;
p22 = p2 + v2n;

% ui = intersect_1;
str = strcat(
	sprintf('(plate\n'),
	sprintf('\t(v %f %f)\n', p1(1), p1(2)),
	sprintf('\t(] %f %f)\n', p11(1), p11(2))
);

u1 = p1 + v1n * r;
u2 = p2 - v2n * r;
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
