%% Load data
stringname='../build/0332-391_parallel_diagnostic';
Y = importdata(stringname);
x = Y.data(:,1);
for i = 2:length(Y.data(1,:))
    figure;
    plot(x, Y.data(:, i));
    xlabel('Iterations') % x-axis label
    ylabel(Y.colheaders(i)) % y-axis label
end