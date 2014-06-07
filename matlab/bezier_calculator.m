% bezier_calculator.m

% // start with the classic bezier curve equation
% P = (1-t)^3*P0 + 3*(1-t)^2*t*P1 + 3*(1-t)*t^2*P2 + t^3*P3
% // convert to polynomial
% P = P0 + 3*t*(P1 - P0) + 3*t^2*(P2 - 2*P1 + P0) + t^3*(P3 - 3*P2 + 3*P1 - P0)
% // rename the terms to a,b,c
% P = P0 + 3at + 3btt + cttt
% // find the derivatives
% P' = 3a + 6bt + 3ctt
% P" = 6b + act
% // and the cross product after some reduction
% P' x P" = ab + act + bctt


%% this script calculates 5 control points
% for given input of 2 points and 2 tangents and 
% a maximum curvature

% f(t) = (1-t)^4*p0+4*(1-t)^3*t*p1+6*(1-t)^2*t^2*p2+4*(1-t)*t^3*p3+t^4*p4
% d/dt =  4*p4*t^3-4*p3*t^3+12*p3*(1-t)*t^2-12*p2*(1-t)*t^2+12*p2*(1-t)^2*t-12*p1*(1-t)^2*t+4*p1*(1-t)^3-4*p0*(1-t)^3
% d^2/dt^2 = 12*p4*t^2-24*p3*t^2+12*p2*t^2+24*p3*(1-t)*t-48*p2*(1-t)*t+24*p1*(1-t)*t+12*p2*(1-t)^2-24*p1*(1-t)^2+12*p0*(1-t)^2
% d^3/dt^3 = 24*p4*t-72*p3*t+72*p2*t-24*p1*t+24*p3*(1-t)-72*p2*(1-t)+72*p1*(1-t)-24*p0*(1-t)

% f(t) = (1-t).^4.*p0+4.*(1-t).^3.*t.*p1+6.*(1-t).^2.*t.^2.*p2+4.*(1-t).*t.^3.*p3+t.^4.*p4
% d/dt =  4.*p4.*t.^3-4.*p3.*t.^3+12.*p3.*(1-t).*t.^2-12.*p2.*(1-t).*t.^2+12.*p2.*(1-t).^2.*t-12.*p1.*(1-t).^2.*t+4.*p1.*(1-t).^3-4.*p0.*(1-t).^3
% d^2/dt^2 = 12.*p4.*t.^2-24.*p3.*t.^2+12.*p2.*t.^2+24.*p3.*(1-t).*t-48.*p2.*(1-t).*t+24.*p1.*(1-t).*t+12.*p2.*(1-t).^2-24.*p1.*(1-t).^2+12.*p0.*(1-t).^2
% d^3/dt^3 = 24.*p4.*t-72.*p3.*t+72.*p2.*t-24.*p1.*t+24.*p3.*(1-t)-72.*p2.*(1-t)+72.*p1.*(1-t)-24.*p0.*(1-t)

% start point / end point
c1 = [0, 0];
c2 = [0, 2];
% tangents
v1 = [1, 0];
v2 = [1, 0];

p0 = c1;
p4 = c2;

p1 = v1/4 + p0;
p3 = - (v2/4 - p4);


t  = 0:0.01:1;
% pdt = 4*(p0.*(t-1).^3-p1.*4.*t-1).*(t-1).^2 + t.*(p2.*(6*t.^2-9*t+3)+t.*(-4*p3.*t+3*p3+p4.*t))
% hold on
% plot(pdt)
% t0 = -(p3-3*p2+3*p1-p0)/(p4-4*p3+6*p2-4*p1+p0)
% p2 = 


% 12*(p0*(((p0-3*p1+3*p2-p3)/(p0-4*p1+6*p2-4*p3+p4))-1)^2+p1*(-4*((p0-3*p1+3*p2-p3)/(p0-4*p1+6*p2-4*p3+p4))^2+6*((p0-3*p1+3*p2-p3)/(p0-4*p1+6*p2-4*p3+p4))-2)+6*p2*((p0-3*p1+3*p2-p3)/(p0-4*p1+6*p2-4*p3+p4))^2-6*p2*((p0-3*p1+3*p2-p3)/(p0-4*p1+6*p2-4*p3+p4))+p2-4*p3*((p0-3*p1+3*p2-p3)/(p0-4*p1+6*p2-4*p3+p4))^2+2*p3*((p0-3*p1+3*p2-p3)/(p0-4*p1+6*p2-4*p3+p4))+p4*((p0-3*p1+3*p2-p3)/(p0-4*p1+6*p2-4*p3+p4))^2)
% 12*(p0*(((p0 - 3 * p1 + 3 * p2 - p3) / (p0 - 4*p1+ 6*p2 - 4*p3 + p4))-1)^2+p1 (-4 ((p0 - 3 * p1 + 3 * p2 - p3) / (p0 - 4*p1+ 6*p2 - 4*p3 + p4))^2+6 ((p0 - 3 * p1 + 3 * p2 - p3) / (p0 - 4*p1+ 6*p2 - 4*p3 + p4))-2)+6 p2 ((p0 - 3 * p1 + 3 * p2 - p3) / (p0 - 4*p1+ 6*p2 - 4*p3 + p4))^2-6 p2 ((p0 - 3 * p1 + 3 * p2 - p3) / (p0 - 4*p1+ 6*p2 - 4*p3 + p4))+p2-4 p3 ((p0 - 3 * p1 + 3 * p2 - p3) / (p0 - 4*p1+ 6*p2 - 4*p3 + p4))^2+2 p3 ((p0 - 3 * p1 + 3 * p2 - p3) / (p0 - 4*p1+ 6*p2 - 4*p3 + p4))+p4 ((p0 - 3 * p1 + 3 * p2 - p3) / (p0 - 4*p1+ 6*p2 - 4*p3 + p4))^2)

% p2 = ?
k = -10;


% p21 = (sqrt(p4.^2+(4*p3-20*p1+14*p0).*p4-8*p3.^2+(32*p1-20*p0).*p3-8*p1.^2+4*p0.*p1+p0.^2)+p4+2*p3+2*p1+p0)/6
% p22 = -(sqrt(p4.^2+(4*p3-20*p1+14*p0).*p4-8*p3.^2+(32*p1-20*p0).*p3-8*p1.^2+4*p0.*p1+p0.^2)-p4-2*p3-2*p1-p0)/6

% p23 = 2*p1 - p0 
% p24 = 2*p3 - p4

% p2 = [5,3];

f = zeros(2, 100);
fd = zeros(2, 100);
fdd= zeros(2, 100);
fddd = zeros(2, 100);
i = 1;

% for t = 0:0.01:1
% 	disp(t)
% 	f(:, i) = (1-t).^4.*p0+4.*(1-t).^3.*t.*p1+6.*(1-t).^2.*t.^2.*p2+4.*(1-t).*t.^3.*p3+t.^4.*p4;
% 	fd(:, i)  = 4.*p4.*t.^3-4.*p3.*t.^3+12.*p3.*(1-t).*t.^2-12.*p2.*(1-t).*t.^2+12.*p2.*(1-t).^2.*t-12.*p1.*(1-t).^2.*t+4.*p1.*(1-t).^3-4.*p0.*(1-t).^3;
% 	fdd(:, i) =12.*p4.*t.^2-24.*p3.*t.^2+12.*p2.*t.^2+24.*p3.*(1-t).*t-48.*p2.*(1-t).*t+24.*p1.*(1-t).*t+12.*p2.*(1-t).^2-24.*p1.*(1-t).^2+12.*p0.*(1-t).^2
% 	fddd(:, i) = 24.*p4.*t-72.*p3.*t+72.*p2.*t-24.*p1.*t+24.*p3.*(1-t)-72.*p2.*(1-t)+72.*p1.*(1-t)-24.*p0.*(1-t)
% 	i = i + 1
% end
% idx = find(fd == max(fd(1, :)))
% figure(1)
% hold on
% axis equal
% plot(f(1, :), f(2, :))
% plot(fd(1, :), fd(2, :), 'k')
% plot(fdd(1, :), fd(2, :), 'g')
% plot(fddd(1, :), fd(2, :), 'r')

% curvature berechnen und numerisch nullstelle der ableitung ermitteln
% und dann limitieren ... 

v1n = v1 / norm(v1)
v2n = v2 / norm(v2)
vm = v1n + (-v2n * 0.5);
vm = vm / norm(vm);
pm = (p0 + (p0 - p4) / 2);
p2 = (vm + (p0 + (p4 - p0) / 2));

p_c  = [p1; p2; p3];

pc = [p0; p_c; p4]

for t=0:0.1:1
	a = 12.*(p0.*(t-1).^2+p1.*(-4.*t.^2+6.*t-2)+6.*p2.*t.^2-6.*p2.*t+p2-4.*p3.*t.^2+2.*p3.*t+p4.*t.^2);
end

% figure(2)
% plot()
figure(2)
axis equal

bezier_curve(pc)