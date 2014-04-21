clf;
p1 = [1, 0]
p2 = [1, 1]
p3 = [0, 0.5]

rotation_p90 = [cos(pi/2)  -sin(pi/2) ; sin(pi/2)  cos(pi/2)]
rotation_m90 = [cos(-pi/2)  -sin(-pi/2) ; sin(-pi/2)  cos(-pi/2)]

v1 = p2 - p1;
v2 = p2 - p3;

radius = 0.2;

v1n = v1 / norm(v1);
v2n = v2 / norm(v2);

angle = acos(dot(v1n, v2n))

deg_angle = angle / pi * 180

l_down = tan(angle / 2) * radius;

interpolate_angle = 0.1 * pi;

v1_m = p2 + (v1n*radius) + (v1n*l_down*rotation_p90);

counter_angle = pi + angle;

rotate_rad = (p2 + v1n * radius) - v1_m;

len_interpol = floor(counter_angle / interpolate_angle)

interpol = zeros(len_interpol, 2);
j = 1;
for i = 0:counter_angle / len_interpol:counter_angle
    rot = [cos(-i)  -sin(-i) ; sin(-i)  cos(-i)];
    interpol(j, :) = rot * rotate_rad';
    j = j + 1;
end

interpol = interpol + repmat(v1_m, len_interpol + 1, 1);

hold on
axis equal
plot([p1(1), p2(1), p3(1)]', [p1(2), p2(2), p3(2)]', 'LineWidth', 3)

pv = [p2 + (v1n*radius); p2; p2 + (v2n*radius)];

plot(pv(:,1), pv(:,2), '-oc', 'LineWidth', 3);
plot(interpol(:,1), interpol(:,2), '-or', 'LineWidth', 3);
