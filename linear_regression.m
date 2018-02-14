function [] = linear_regression(d)
    train_feature_fn = ['HCEP-50K-dKSeries/50K.train.dk-features.', num2str(d)];
    test_feature_fn = ['HCEP-50K-dKSeries/50K.test.dk-features.', num2str(d)];
    val_feature_fn = ['HCEP-50K-dKSeries/50K.val.dk-features.', num2str(d)];
    
    train_predict_fn = ['HCEP-50K-predict/50K.train.predict.', num2str(d), '.mat'];
    test_predict_fn = ['HCEP-50K-predict/50K.test.predict.', num2str(d), '.mat'];
    val_predict_fn = ['HCEP-50K-predict/50K.val.predict.', num2str(d), '.mat'];
    
    train_target_fn = 'HCEP-50K/50K.train.pce';
    test_target_fn = 'HCEP-50K/50K.test.pce';
    val_target_fn = 'HCEP-50K/50K.val.pce';
    
    train_feature = load(train_feature_fn);
    test_feature = load(test_feature_fn);
    val_feature = load(val_feature_fn);
    
    train_target = load(train_target_fn);
    test_target = load(test_target_fn);
    val_target = load(val_target_fn);
    
    train_target = train_target(2:end);
    test_target = test_target(2:end);
    val_target = val_target(2:end);
    
    function [mae] = MAE(predict, target)
        mae = mean(abs(predict - target));
    end

    function [rmse] = RMSE(predict, target)
        rmse = mean((predict - target) .* (predict - target));
    end
    
    lambda_list = [1e-8, 1e-6, 1e-4, 1e-2, 1, 10, 100];
    for ind = 1:size(lambda_list, 2)
        lambda = lambda_list(ind);
        w = (train_feature' * train_feature + lambda * eye(size(train_feature, 2)))^-1 * train_feature' * train_target;
        
        train_predict = train_feature * w;
        val_predict = val_feature * w;
        
        train_mae = MAE(train_predict, train_target);
        train_rmse = RMSE(train_predict, train_target);
        
        val_mae = MAE(val_predict, val_target);
        val_rmse = RMSE(val_predict, val_target);
        
        if ind == 1
            best_lambda = lambda;
            best_w = w;
            best_mae = val_mae;
            best_rmse = val_rmse;
        else
            if best_mae > val_mae
                best_lambda = lambda;
                best_w = w;
                best_mae = val_mae;
                best_rmse = val_rmse;
            end
        end
        
        fprintf('--- lambda = %.6f ---------------\n', lambda);
        fprintf('Train MAE = %.6f\n', train_mae);
        fprintf('Train RMSE = %.6f\n', train_rmse);
        fprintf('Validation MAE = %.6f\n', val_mae);
        fprintf('Validation RMSE = %.6f\n', val_rmse);
    end
    
    fprintf('--- Summary ----------------------------\n');
    
    train_predict = train_feature * best_w;
    val_predict = val_feature * best_w;
    test_predict = test_feature * best_w;
    
    test_mae = MAE(test_predict, test_target);
    test_rmse = RMSE(test_predict, test_target);
    
    fprintf('Best lambda = %.6f\n', best_lambda);
    fprintf('Test MAE = %.6f\n', test_mae);
    fprintf('Test RMSE = %.6f\n', test_rmse);   
    
    save(train_predict_fn, 'train_predict'); 
    save(test_predict_fn, 'test_predict');
    save(val_predict_fn, 'val_predict');
end