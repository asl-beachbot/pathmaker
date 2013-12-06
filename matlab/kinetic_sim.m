%% A matlab file that tries to simulate the kinetics of the roboter
%  Variables:
%       robot = [x, y, theta]: The position of the 
%                              turning point of the trike
%       rake = [x, y]: The position of the rake
%       l = the distance from turning point to rake

isOctave = exist('OCTAVE_VERSION') ~= 0;
if isOctave
    setenv("GNUTERM", "wxt")
end

clf

init_pos = [0, 0]
l = 2

%% Calculate tilted rectangle
%  

function [] = draw_tilted_rect(pos, s, theta)
    disp('drawing rect')
    r = [- s(1:2);
         - s(1), s(2);
         s(1:2);
         s(1), - s(2)];
    rot = [cos(theta), sin(theta); -sin(theta), cos(theta)];
    r = (rot * r')';
    r = pos + r;
    r(5,:) = r(1,:)
    axis equal
    plot(r(:,1), r(:, 2))
end

draw_tilted_rect([0,0], [4,8], pi/2)
pause()
