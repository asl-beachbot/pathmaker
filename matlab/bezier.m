% Function to calculate and plot a bezier curve from control points
% Input
%       P: 2xL matrix with L 2D control points
%       t: 1xR t [0..1] defines resolution of the plot linspace(0, 1, 100)
%
% Output: Plots and Q (2xR) 


function Q = bezier(P, t)
    Q(:,1) = [0, 0]';
    %plot(P(1,:), P(2,:), 'ro')
    % P(:, size(P, 2)) = P(:, 1);
    parfor k = 1:length(t)
        Q(:, k) = [0,0]';
        for j = 1:size(P,2)
            Q(:,k) = Q(:,k) + P(:,j) * bernstein( size(P,2) - 1, j - 1, t(k) );
        end
    end
    plot(Q(1,:), Q(2, :));
end

function B = bernstein(n, i, t)
    B = factorial(n) / (factorial(i) * factorial(n - i)) * t^i * (1-t)^(n-i);
end