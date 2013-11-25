% Function to calculate and plot a bezier curve from control points
% Input
%       P: 2xL matrix with L 2D control points
%       t: 1xR t [0..1] defines resolution of the plot linspace(0, 1, 100)
%
% Output: Plots and Q (2xR) 


function Q = bezier(P, t, v=false)
    Q(:,1) = [0, 0]';
    %plot(P(1,:), P(2,:), 'ro')
    % P(:, size(P, 2)) = P(:, 1);
    parfor k = 1:length(t)
        Q(:, k) = [0,0]';
        for j = 1:size(P,2)
            Q(:,k) = Q(:,k) + P(:,j) * bernstein( size(P,2) - 1, j - 1, t(k) );
        end
    end
    %% Plotting the vehicle path
    vehiclepath = [0, 0]';
    v = [0, 0]';
    for i = 1:(length(t) - 1)
        v = Q(:, i+1) - Q(:, i) 
        v = (v / norm(v)) * 1
        vehiclepath(:, i) = (Q(:, i) + v)
        vehiclepath(1, i) = vehiclepath(1, i)
    end
    
    vct_path = [0,0]';
    for i = 1:2:2 * (length(t) - 2)
        vct_path(:, i) = Q(:, ceil(i / 2));
        vct_path(:, i + 1) = vehiclepath(:, ceil(i / 2));
    end
    plot(Q(1,:), Q(2, :));
    plot(vct_path(1, :), vct_path(2, :), 'c');
    plot(vehiclepath(1, :), vehiclepath(2, :), 'k');
end

function B = bernstein(n, i, t)
    B = factorial(n) / (factorial(i) * factorial(n - i)) * t^i * (1-t)^(n-i);
end