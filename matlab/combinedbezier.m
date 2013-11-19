% plot combined bezier curve

function combinedbezier (P)
    hold on
    [sx, sy, sz] = size(P);
    for i = 1:sx
        j = P(i, :, :);
        a = squeeze(j)';
        bezier(a, linspace(0,1, 10));
        plot(a(1, :)', a(2, :)', 'ro')
    end
    hold off
end