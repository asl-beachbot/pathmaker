p1 = [0, 0]
p2 = [1, 1]
p3 = [0, 4]

rotation_p90 = [cos(pi/2)  -sin(pi/2) ; sin(pi/2)  cos(pi/2)]
rotation_m90 = [cos(-pi/2)  -sin(-pi/2) ; sin(-pi/2)  cos(-pi/2)]

v1 = p2 - p1;
v2 = p2 - p3;

v1n = v1 / norm(v1);
v2n = v2 / norm(v2);

angle = acos(dot(v1n, v2n))

deg_angle = angle / pi * 180



v1_m = p2 - (v1n*0.5) - (v1n*0.5*rotation_p90);

hold on
plot([p1(1), p2(1), p3(1)]', [p1(2), p2(2), p3(2)]')
pv = [p2 - (v1n*0.5); v1_m; p2 - (v2n*0.5)];
plot(pv(:,1), pv(:,2), '-oc');