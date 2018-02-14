function [] = multi_histograms(d)
    expected = load('HCEP-50K/50K.test.pce');
    expected = expected(2:end);
        
    load(['HCEP-50K-predict/50K.test.predict.', num2str(d), '.mat'], 'test_predict');
    
    mae = mean(abs(test_predict - expected));
    rmse = sqrt(mean((test_predict - expected) .* (test_predict - expected)));
    
    fprintf('MAE: %.6f\n', mae);
    fprintf('RMSE: %.6f\n', rmse);
    
    range = min(min(expected), min(test_predict)) : 0.25 : max(max(expected), max(test_predict));
    
    figure(1);
    hist(test_predict, range, 20);
    h = findobj(gca, 'Type', 'patch');
    set(h, 'FaceColor', 'r', 'EdgeColor', 'w', 'facealpha', 0.75);
    hold on;
    hist(expected, range, 20);
    h = findobj(gca, 'Type', 'patch');
    set(h, 'facealpha', 0.75);
    xlabel('PCE')
    ylabel('Number of appearances')
    legend('Predicted', 'Expected')
    title(['dK-Series d = ', num2str(d),', MAE = ', num2str(mae), ', RMSE = ', num2str(rmse)]);
    hold off;
end