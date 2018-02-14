% Program: PCA visualization
% Author: Hy Truong Son
% Major: PhD Computer Science
% School: Department of Computer Science, The University of Chicago
% Email: hytruongson@uchicago.edu

function [] = PCA_visual(d)
    %% Dataset
    title_name = ['PCA - dK Series features - d = ', num2str(d)];
    feature_fn = ['HCEP-50K-dKSeries/50K.test.dk-features.', num2str(d)];
    label_fn = 'HCEP-50K/50K.test.pce';
    
    %% Loading data
    rng(0);    
    feature = load(feature_fn);
    
    label = load(label_fn);
    label = label(2:end);
    label = round(label);
    
    %% PCA 
    C = feature' * feature;
    [E, D] = eig(C);
    
    N = size(feature, 2);
    for i = 1 : N
        for j = i + 1 : N
            if D(i, i) < D(j, j)
                temp = D(i, i);
                D(i, i) = D(j, j);
                D(j, j) = temp;
                
                temp = E(:, i);
                E(:, i) = E(:, j);
                E(:, j) = temp;
            end
        end
    end
    
    pca_feature = feature * [E(:, 1), E(:, 2)];
    
    figure(1);
    d = diag(D);
    plot(1:N, d, 'r');
    xlabel('Number of eigenvalues');
    ylabel('Eigenvalues');
    title(title_name);
    
    figure(2);
    imagesc(feature);
    title(title_name);
    
    figure(3);
    gscatter(pca_feature(:, 1), pca_feature(:, 2), label(1:end));
    title(title_name);
end
