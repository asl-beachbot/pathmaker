img = imread('../assets/testpath.png');
img = ~im2bw(img, 0.5);
bw_rem = bwmorph(img,'remove');

[c_x, c_y] = find(bw_rem == 1);

A = [c_x, c_y];

[~,rm,~] = unique(A(:,2), 'stable');

uniques = A(rm, :);

plot(A(:, 1)', A(:, 2)', 'yo')
[c_x, c_y] = points2contour(c_x, c_y, 1, 'cw');
 
A = [c_x', c_y'];

plot(A(:, 1)', A(:, 2)', 'ro')

% remove every nth point
% stupid approach
% n = 2;
% A = A(1:n:end, :); 

% remove every point with redundant y value
% to be implemented

disp(A);

res_bez = mbezier(A);
 
combinedbezier(res_bez);