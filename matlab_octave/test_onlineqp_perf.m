oqp_list = readdir('onlineqp_collection/oqp');
problems_list = readdir('onlineqp_collection/problems');

dir_list = {
    strcat('onlineqp_collection/oqp/', oqp_list(!(strcmp(oqp_list, '..') | strcmp(oqp_list, '.')))){:}, ...
    strcat('onlineqp_collection/problems/', problems_list(!(strcmp(problems_list, '..') | strcmp(problems_list, '.')))){:}  };

qpmad_time = [];
qpoases_time = [];
quadprogpp_time = [];

for i = 1:numel(dir_list)
    dims = load([dir_list{i}, '/dims.oqp']);
    number_qp = dims(1);
    number_general_ctr  = dims(3);

    H  = load([dir_list{i}, '/H.oqp']);
    g  = load([dir_list{i}, '/g.oqp']);


    if (min(eig(H)) < 1e-13)
        printf('SKIP    Problem [%s] (%d) // SKIPPED: Hessian is not positive definite\n', dir_list{i}, i);
        continue;
    end


    % bounds
    lb = load([dir_list{i}, '/lb.oqp']);
    ub = load([dir_list{i}, '/ub.oqp']);

    % general constraints
    if ( number_general_ctr > 0 )
        Ain  = load([dir_list{i}, '/A.oqp']);
        lbin = load([dir_list{i}, '/lbA.oqp']);
        ubin = load([dir_list{i}, '/ubA.oqp']);
    else
        Ain  = [];
        lbin = [];
        ubin = [];
    end

    % solutions
    x_ref = load([dir_list{i}, '/x_opt.oqp']);


    for j = 1:number_qp
        printf('Problem [%s] (%d) // [%d/%d]\n', dir_list{i}, i, j, number_qp);

        tic()
        if (number_general_ctr > 0)
            [x, info] = qpmad(H, g(j,:)', [], [], lb(j, :)', ub(j, :)', Ain, lbin(j,:)', ubin(j,:)');
        else
            [x, info] = qpmad(H, g(j,:)', [], [], lb(j, :)', ub(j, :)');
        end
        qpmad_time = [qpmad_time, toc()];


        %options = qpOASES_options( 'default' );
        options = qpOASES_options( 'MPC' );
        tic();
        if (number_general_ctr > 0)
            [x_qpoases,fval,exitflag,iter,lambda,auxOutput] = ...
                qpOASES(H, g(j,:)', Ain, lb(j, :)', ub(j, :)', lbin(j,:)', ubin(j,:)', options);
        else
            [x_qpoases,fval,exitflag,iter,lambda,auxOutput] = ...
                qpOASES(H, g(j,:)', lb(j, :)', ub(j, :)', options);
        end
        qpoases_time = [qpoases_time, toc()];


        tic()
        if (number_general_ctr > 0)
            [x, info] = quadprogpp(H, g(j,:)', [], [], lb(j, :)', ub(j, :)', Ain, lbin(j,:)', ubin(j,:)');
        else
            [x, info] = quadprogpp(H, g(j,:)', [], [], lb(j, :)', ub(j, :)');
        end
        quadprogpp_time = [quadprogpp_time, toc()];
    end
end

figure
hold on
plot(qpmad_time, 'r')
plot(qpoases_time, 'b')
plot(quadprogpp_time, 'k')
hold off
legend ('qpmad', 'qpoases', 'quadprogpp')
