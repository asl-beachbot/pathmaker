% Algorithm for finding largest rectangle
% which is aligned with one side
% Limitations: 
%   Can only work on 4 sided polygons
%
% Function:
%   Test side: both angles > 180
%   not longest distance
rect = [
    1,1;
    1,6;
    5,6;
    4,0;
];
hold on
plot(rect(:,1), rect(:,2))
rect_inner = zeros(4, 2)
for i=0:3
    i + 1
    mod(i + 1, 4) + 1
    mod(i + 2, 4) + 1
    v1 = rect(mod(i + 1, 4) + 1, :) - rect(i + 1, :)
    v2 = rect(mod(i + 1, 4) + 1, :) - rect(mod(i + 2, 4) + 1, :)
    v3 = rect(mod(i + 3, 4) + 1, :) - rect(mod(i + 2, 4) + 1, :);
    p3 = rect(mod(i + 3, 4) + 1, :);
    p3_index = mod(i + 3, 4)
    v1_n = v1 / norm(v1);
    v2_n = v2 / norm(v2);
    v3_n = v3 / norm(v3);
    alpha = acos(dot(v1_n, v2_n))
    if( alpha > pi/2 )
        disp("adjusting")
        alpha_ub = alpha - pi/2
        len = sin(alpha_ub) * norm(v2)
        v3_minus = v3_n * len
        p3 = p3 - v3_minus
        rect_inner(p3_index, :) = p3
    else
        rect_inner(i + 1, :) = p3
    end
end
rect_inner = rect_inner + ones(4, 2)
plot(rect_inner(:, 1), rect_inner(:, 2), "r")
