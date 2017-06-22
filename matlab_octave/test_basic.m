tolerance = 1e-08;

function x = check_result(test_id, condition)
    if (condition)
        printf('Test %d OK\n', test_id);
    else
        printf('Test %d FAIL\n', test_id);
    end
end

TEST_ID = 1;


%-------------------------------------------------
% [x, info] = qpmad (H)

N = 20;
[x, info] = qpmad(eye(N));
xref = zeros(N,1);

check_result(TEST_ID, (abs(sum(x-xref)) < tolerance) && (info.status == 0))
TEST_ID = TEST_ID + 1;
%-------------------------------------------------


%-------------------------------------------------
% [x, info] = qpmad (H, g)

N = 20;
[x, info] = qpmad(eye(N), ones(N, 1));
xref = -1*ones(N,1);

check_result(TEST_ID, (abs(sum(x-xref)) < tolerance) && (info.status == 0))
TEST_ID = TEST_ID + 1;
%-------------------------------------------------


%-------------------------------------------------
% [x, info] = qpmad (H, g, A, b)

N = 20;
[x, info] = qpmad(eye(N), ones(N, 1), [eye(4), zeros(4, N-4)], [1; 2; 3; 4]);
xref = [1   2   3   4  -1  -1  -1  -1  -1  -1  -1  -1  -1  -1  -1  -1  -1  -1  -1  -1]';

check_result(TEST_ID, (abs(sum(x-xref)) < tolerance) && (info.status == 0))
TEST_ID = TEST_ID + 1;
%-------------------------------------------------


%-------------------------------------------------
% [x, info] = qpmad (H, g, A, b, lb, ub) {infeasible}

N = 20;
[x, info] = qpmad( eye(N),
                        ones(N, 1),
                        [eye(4), zeros(4, N-4)],
                        [1; 2; 3; 4],
                        -100*ones(N,1),
                        zeros(N,1));

check_result(TEST_ID, info.status > 0)
TEST_ID = TEST_ID + 1;
%-------------------------------------------------


%-------------------------------------------------
% [x, info] = qpmad (H, g, A, b, lb, ub)

N = 20;
[x, info] = qpmad( eye(N),
                        ones(N, 1),
                        [eye(4), zeros(4, N-4)],
                        [1; 2; 3; 4],
                        [-100*ones(4,1); -0.5*ones(N-4, 1)],
                        [100*ones(4,1); 0.5*ones(N-4, 1)]);
xref = [1.0   2.0   3.0   4.0  -0.5  -0.5 -0.5  -0.5  -0.5  -0.5  -0.5  -0.5 -0.5  -0.5  -0.5  -0.5  -0.5  -0.5 -0.5  -0.5]';


check_result(TEST_ID, (abs(sum(x-xref)) < tolerance) && (info.status == 0))
TEST_ID = TEST_ID + 1;
%-------------------------------------------------


%-------------------------------------------------
% [x, info] = qpmad (H, g, A, b, lb, ub, Ain, lbin, ubin)

N = 20;
[x, info] = qpmad( eye(N),
                        ones(N, 1),
                        [eye(4), zeros(4, N-4)],
                        [1; 2; 3; 4],
                        [-100*ones(4,1); -5*ones(N-4, 1)],
                        [100*ones(4,1); 0.5*ones(N-4, 1)],
                        ones(1,N),
                        [-1.5],
                        [1.5]);
xref = [1.0   2.0   3.0   4.0  -0.71875  -0.71875 -0.71875  -0.71875  -0.71875  -0.71875  -0.71875  -0.71875 -0.71875  -0.71875  -0.71875  -0.71875  -0.71875  -0.71875 -0.71875  -0.71875]';

check_result(TEST_ID, (abs(sum(x-xref)) < tolerance) && (info.status == 0))
TEST_ID = TEST_ID + 1;
%-------------------------------------------------
