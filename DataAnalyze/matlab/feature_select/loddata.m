function out=loddata(param, method)
    % 全局变量声明
    global S P_train T_train P_test T_test
    out = 1;
    
    if strcmp(param,'random_fun')==1;
        load 'G:\game\maniadrive\ManiaDrive_Custom\数据分析\matlab\feature_select\fun_t_s_random.mat';
        level_track_mat = fun_track_statistic_mat;
        [m,n] = size(level_track_mat);
        %随机选择样本
        %a=randperm(236);
        Train=level_track_mat(:,1:S+1);
        Test=level_track_mat(201:end,1:S+1);
        
    elseif strcmp(param, 'classed_fun')==1;
        load 'G:\game\maniadrive\ManiaDrive_Custom\数据分析\matlab\feature_select\fun_t_s_classed.mat';
        level_track_mat = fun_track_statistic_mat;
        Train = [level_track_mat(7:47,1:S+1); level_track_mat(61:101,1:S+1);level_track_mat(175:215,1:S+1)];
        Test = [level_track_mat(48:58,1:S+1); level_track_mat(102:112,1:S+1);level_track_mat(216:226,1:S+1)];
        
    elseif strcmp(param, 'classedrandom_fun')==1;
        load 'G:\game\maniadrive\ManiaDrive_Custom\数据分析\matlab\feature_select\fun_t_s_classed.mat';
        level_track_mat = fun_track_statistic_mat;
        Train = [level_track_mat(7:47,1:S+1); level_track_mat(61:101,1:S+1);level_track_mat(175:215,1:S+1)];
        Test = [level_track_mat(48:58,1:S+1); level_track_mat(102:112,1:S+1);level_track_mat(216:226,1:S+1)];
        [m,n] = size(Train);
        a = randperm(m);

        Train = Train(a(:),:);
    elseif strcmp(param, 'random_feel')==1;
        load 'G:\game\maniadrive\ManiaDrive_Custom\数据分析\matlab\feature_select\feel_l_t_s.mat';
        level_track_mat = feel_l_t_s_mat;
        Train=level_track_mat(1:200,1:S+1);
        Test=level_track_mat(201:end,1:S+1);
        
    elseif strcmp(param, 'level_prefer')==1;
        load 'G:\game\maniadrive\ManiaDrive_Custom\数据分析\matlab\feature_select\fun_prefer_l_s.mat';
        level_track_mat = prefer_level_mat;
        Train=level_track_mat(:,1:S+1);
        Test=level_track_mat(100:end,1:S+1);
    elseif strcmp(param, 'difficult_random')==1;
        load  'G:\game\maniadrive\ManiaDrive_Custom\数据分析\matlab\feature_select\difficult_t_s_random.mat';
        level_track_mat = difficult_mat;
        Train=level_track_mat(:,1:S+1);
        Test=level_track_mat(100:end,1:S+1);
    elseif strcmp(param, 'skill_random')==1;
        load  'G:\game\maniadrive\ManiaDrive_Custom\数据分析\matlab\feature_select\skill_t_s_random.mat';
        level_track_mat = skill_mat;
        Train=level_track_mat(:,1:S+1);
        Test=level_track_mat(100:end,1:S+1);
    elseif strcmp(param, 'wzm_prefer')==1;
        load 'G:\game\maniadrive\ManiaDrive_Custom\数据分析\matlab\feature_select\wzm_prefer.mat';
        level_track_mat = wzm_prefer_mat;
        Train=level_track_mat(:,1:S+1);
        Test=level_track_mat(100:end,1:S+1);
    else
        disp('wrong param');
        out = 0;
    end
    
    if out ~= 0;
        P_train=Train(:,2:end);
        T_train=Train(:,1);
        % 测试数据
        P_test=Test(:,2:end);
        T_test=Test(:,1);
        
        if nargin<2
            method = 'nor';
        end
        
        if strcmp(method,'nor')==1;
            %数据归一化
            [P_train_scale,ps] = mapminmax(P_train');
            ps.ymin = 0;
            ps.ymax = 1;
            [P_train_scale,ps] = mapminmax(P_train', ps);
            P_train = P_train_scale';

            [P_test_scale, ps] = mapminmax(P_test');
            ps.ymin = 0;
            ps.ymax = 1;
            [P_test_scale,ps] = mapminmax(P_test', ps);
            P_test = P_test_scale';
        elseif strcmp(method,'std')==1;
            P_train = zscore(P_train);
            P_test = zscore(P_test);
            %{
            [m,n] = size(P_train);
            for i=1:n
                col = P_train(:,i);
                col_mean = mean(col);
                col_std = std(col);
                col = (col - col_mean) / col_std;
            end
            %}
        elseif strcmp(method,'std_pca')==1;
            P_train = zscore(P_train);
            P_test = zscore(P_test);
            mpca = pca(P_train');
            P_train = linproj(P_train', mpca);
            P_train = P_train';
        elseif strcmp(method,'std_nor')==1;
            P_train = zscore(P_train);
            P_test = zscore(P_test);
            
            %数据归一化
            [P_train_scale,ps] = mapminmax(P_train');
            ps.ymin = 0;
            ps.ymax = 1;
            [P_train_scale,ps] = mapminmax(P_train', ps);
            P_train = P_train_scale';

            [P_test_scale, ps] = mapminmax(P_test');
            ps.ymin = 0;
            ps.ymax = 1;
            [P_test_scale,ps] = mapminmax(P_test', ps);
            P_test = P_test_scale';
        elseif strcmp(method,'nor_pca')==1;
            [P_train_scale,ps] = mapminmax(P_train');
            ps.ymin = 0;
            ps.ymax = 1;
            [P_train_scale,ps] = mapminmax(P_train', ps);
            P_train = P_train_scale';

            [P_test_scale, ps] = mapminmax(P_test');
            ps.ymin = 0;
            ps.ymax = 1;
            [P_test_scale,ps] = mapminmax(P_test', ps);
            P_test = P_test_scale';
            mpca = pca(P_train');
            P_train = linproj(P_train', mpca);
            P_train = P_train';
        end
    end
end