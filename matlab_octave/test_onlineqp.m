oqp_list = readdir('onlineqp_collection/oqp');
problems_list = readdir('onlineqp_collection/problems');
tolerance = 1e-09;

dir_list = {
    strcat('onlineqp_collection/oqp/', oqp_list(!(strcmp(oqp_list, '..') | strcmp(oqp_list, '.')))){:}, ...
    strcat('onlineqp_collection/problems/', problems_list(!(strcmp(problems_list, '..') | strcmp(problems_list, '.')))){:}  };


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
    if (exist([dir_list{i}, '/obj_opt.oqp'], 'file'))
        obj_ref = load([dir_list{i}, '/obj_opt.oqp']);
    else
        obj_ref = NaN;
    end

    for j = 1:number_qp
        if (number_general_ctr > 0)
            [x, info] = qpmad(H, g(j,:)', [], [], lb(j, :)', ub(j, :)', Ain, lbin(j,:)', ubin(j,:)');
        else
            [x, info] = qpmad(H, g(j,:)', [], [], lb(j, :)', ub(j, :)');
        end

        if ((norm(x-x_ref(j,:)') < tolerance) && (info.status == 0))
            printf('OK      Problem [%s] (%d) // [%d/%d]\n', dir_list{i}, i, j, number_qp);
        else
            printf('FAIL    Problem [%s] (%d) // [%d/%d]\n', dir_list{i}, i, j, number_qp);

            % feasibility check
            fail = 0;
            ind = (lb(j, :)' > x);
            if (norm((lb(j, :)' - x)(ind)) > tolerance)
                printf('    !!! Violated lower bounds\n');
                fail = 1;
            end

            ind = (ub(j, :)' < x);
            if (norm((ub(j, :)' - x)(ind)) > tolerance)
                printf('    !!! Violated upper bounds\n');
                fail = 1;
            end

            if (number_general_ctr > 0)
                ind = (lbin(j, :)' > Ain*x);
                if (norm((lbin(j, :)' - Ain*x)(ind)) > tolerance)
                    printf('    !!! Violated lower bounds of general constraints\n');
                    fail = 1;
                end

                ind = (ubin(j, :)' < Ain*x);
                if (norm((ubin(j, :)' - Ain*x)(ind)) > tolerance)
                    printf('    !!! Violated upper bounds of general constraints\n');
                    fail = 1;
                end
            end

            % optimality check
            if (0.5*x'*H*x + g(j,:)*x > obj_ref(j))
                printf('    !!! Suboptimal solution\n');
                fail = 1;
            end

            %options = qpOASES_options( 'default' );
            options = qpOASES_options( 'MPC' );
            if (number_general_ctr > 0)
                [x_qpoases,fval,exitflag,iter,lambda,auxOutput] = ...
                    qpOASES(H, g(j,:)', Ain, lb(j, :)', ub(j, :)', lbin(j,:)', ubin(j,:)', options);
            else
                [x_qpoases,fval,exitflag,iter,lambda,auxOutput] = ...
                    qpOASES(H, g(j,:)', lb(j, :)', ub(j, :)', options);
            end

            keyboard
        end
    end
end
