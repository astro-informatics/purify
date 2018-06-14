[filename, pathname] = ...
     uigetfile({'*.*'},'File Selector');
Y = importdata(fullfile(pathname, filename));
x = Y.data(:,1);
figure;
for i = 2:length(Y.data(1,:))
    %subplot(length(Y.data(1,:))/2,2,i - 1);
    figure;
    y = Y.data(:, i);
    if (max(abs(y))/min(abs(y)) > 10)
        semilogy(x, y);
    end;
    if (max(abs(y))/min(abs(y)) <= 10)
        plot(x, y);
    end;
    xlabel('Iterations') % x-axis label
    ylabel(Y.colheaders(i)) % y-axis label
end