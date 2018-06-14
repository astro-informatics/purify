set(groot,'ScreenPixelsPerInch',90);

col = hot(256); tmp = linspace(0,1,256)';
for n = 1:3, col(:,n) = interp1( 10.^tmp, col(:,n), 1+9*tmp, 'linear'); end

box = [1, 1, 256, 256];
path = '/Users/luke/dev/purify/build/outputs/';
%%%%%%%%%%

name = '30dor_256_input';
stringname = [path, name];
data = fitsread([stringname, '.fits']);

h = figure;
set(gca, 'LooseInset', get(gca, 'TightInset'));
imagesc(flipud(data(box(1):box(3),box(2):box(4))));
caxis([0,0.2])
set(h,'Units','Inches');
pos = get(h,'Position');

colormap(col);
c = colorbar;


set(gca,'DataAspectRatio',[1 1 1])
set(gca,'visible','off')
set(h,'PaperPositionMode','Auto','PaperUnits','Inches','PaperSize',[pos(3), pos(4)])
print(h, [name, '.pdf'],'-dpdf','-r0')
%%%%%%%%%%

name = '30dor_25630_box_solution';
stringname = [path, name];
data = fitsread([stringname, '.fits']);

h = figure;
set(gca, 'LooseInset', get(gca, 'TightInset'));
imagesc(flipud(data(box(1):box(3),box(2):box(4))));
caxis([0,0.2])
set(h,'Units','Inches');
pos = get(h,'Position');

colormap(col);
c = colorbar;


set(gca,'DataAspectRatio',[1 1 1])
set(gca,'visible','off')
set(h,'PaperPositionMode','Auto','PaperUnits','Inches','PaperSize',[pos(3), pos(4)])
print(h, [name, '.pdf'],'-dpdf','-r0')
%%%%%%%%%%

name = '30dor_25630_kb_solution';
stringname = [path, name];
data = fitsread([stringname, '.fits']);

h = figure;
set(gca, 'LooseInset', get(gca, 'TightInset'));
imagesc(flipud(data(box(1):box(3),box(2):box(4))));
caxis([0,0.2])
set(h,'Units','Inches');
pos = get(h,'Position');

colormap(col);
c = colorbar;


set(gca,'DataAspectRatio',[1 1 1])
set(gca,'visible','off')
set(h,'PaperPositionMode','Auto','PaperUnits','Inches','PaperSize',[pos(3), pos(4)])
print(h, [name, '.pdf'],'-dpdf','-r0')
%%%%%%%%%%

name = '30dor_25630_kb_dirty';
stringname = [path, name];
data = fitsread([stringname, '.fits']);

h = figure;
set(gca, 'LooseInset', get(gca, 'TightInset'));
imagesc(flipud(data(box(1):box(3),box(2):box(4))));
caxis([-0.05, 0.2])
set(h,'Units','Inches');
pos = get(h,'Position');

colormap(col);
c = colorbar;


set(gca,'DataAspectRatio',[1 1 1])
set(gca,'visible','off')
set(h,'PaperPositionMode','Auto','PaperUnits','Inches','PaperSize',[pos(3), pos(4)])
print(h, [name, '.pdf'],'-dpdf','-r0')
%%%%%%%%%%

name = '30dor_25630_box_residual';
stringname = [path, name];
data = fitsread([stringname, '.fits']);

h = figure;
set(gca, 'LooseInset', get(gca, 'TightInset'));
imagesc(flipud(data(box(1):box(3),box(2):box(4))));
caxis([-0.01,0.01])
set(h,'Units','Inches');
pos = get(h,'Position');

colormap(col);
c = colorbar;


set(gca,'DataAspectRatio',[1 1 1])
set(gca,'visible','off')
set(h,'PaperPositionMode','Auto','PaperUnits','Inches','PaperSize',[pos(3), pos(4)])
print(h, [name, '.pdf'],'-dpdf','-r0')
%%%%%%%%%%

name = '30dor_25630_kb_residual';
stringname = [path, name];
data = fitsread([stringname, '.fits']);

h = figure;
set(gca, 'LooseInset', get(gca, 'TightInset'));
imagesc(flipud(data(box(1):box(3),box(2):box(4))));
caxis([-0.01,0.01])
set(h,'Units','Inches');
pos = get(h,'Position');

colormap(col);
c = colorbar;


set(gca,'DataAspectRatio',[1 1 1])
set(gca,'visible','off')
set(h,'PaperPositionMode','Auto','PaperUnits','Inches','PaperSize',[pos(3), pos(4)])
print(h, [name, '.pdf'],'-dpdf','-r0')

%%%%%%%%%%

name1 = '30dor_256_input';
stringname = [path, name1];
data1 = fitsread([stringname, '.fits']);

name2 = '30dor_25630_kb_solution';
stringname = [path, name2];
data2 = fitsread([stringname, '.fits']);

name = '30dor_25630_kb_error';
data = data1 - data2;

h = figure;
set(gca, 'LooseInset', get(gca, 'TightInset'));
imagesc(flipud(data(box(1):box(3),box(2):box(4))));

set(h,'Units','Inches');
pos = get(h,'Position');
caxis([-0.1,0.1])
colormap(col);
c = colorbar;


set(gca,'DataAspectRatio',[1 1 1])
set(gca,'visible','off')
set(h,'PaperPositionMode','Auto','PaperUnits','Inches','PaperSize',[pos(3), pos(4)])
print(h, [name, '.pdf'],'-dpdf','-r0')
%%%%%%%%%%

%%%%%%%%%%

name1 = '30dor_256_input';
stringname = [path, name1];
data1 = fitsread([stringname, '.fits']);

name2 = '30dor_25630_box_solution';
stringname = [path, name2];
data2 = fitsread([stringname, '.fits']);

name = '30dor_25630_box_error';
data = data1 - data2;

h = figure;
set(gca, 'LooseInset', get(gca, 'TightInset'));
imagesc(flipud(data(box(1):box(3),box(2):box(4))));

set(h,'Units','Inches');
pos = get(h,'Position');
caxis([-0.1,0.1])
colormap(col);
c = colorbar;


set(gca,'DataAspectRatio',[1 1 1])
set(gca,'visible','off')
set(h,'PaperPositionMode','Auto','PaperUnits','Inches','PaperSize',[pos(3), pos(4)])
print(h, [name, '.pdf'],'-dpdf','-r0')
%%%%%%%%%%

name = 'M31_input';
stringname = [path, name];
data = fitsread([stringname, '.fits']);

h = figure;
set(gca, 'LooseInset', get(gca, 'TightInset'));
imagesc(flipud(data(box(1):box(3),box(2):box(4))));
caxis([0,0.2])
set(h,'Units','Inches');
pos = get(h,'Position');

colormap(col);
c = colorbar;


set(gca,'DataAspectRatio',[1 1 1])
set(gca,'visible','off')
set(h,'PaperPositionMode','Auto','PaperUnits','Inches','PaperSize',[pos(3), pos(4)])
print(h, [name, '.pdf'],'-dpdf','-r0')
%%%%%%%%%%

name = 'M3130_box_solution';
stringname = [path, name];
data = fitsread([stringname, '.fits']);

h = figure;
set(gca, 'LooseInset', get(gca, 'TightInset'));
imagesc(flipud(data(box(1):box(3),box(2):box(4))));
caxis([0,0.2])
set(h,'Units','Inches');
pos = get(h,'Position');

colormap(col);
c = colorbar;


set(gca,'DataAspectRatio',[1 1 1])
set(gca,'visible','off')
set(h,'PaperPositionMode','Auto','PaperUnits','Inches','PaperSize',[pos(3), pos(4)])
print(h, [name, '.pdf'],'-dpdf','-r0')
%%%%%%%%%%

name = 'M3130_kb_solution';
stringname = [path, name];
data = fitsread([stringname, '.fits']);

h = figure;
set(gca, 'LooseInset', get(gca, 'TightInset'));
imagesc(flipud(data(box(1):box(3),box(2):box(4))));
caxis([0,0.2])
set(h,'Units','Inches');
pos = get(h,'Position');

colormap(col);
c = colorbar;


set(gca,'DataAspectRatio',[1 1 1])
set(gca,'visible','off')
set(h,'PaperPositionMode','Auto','PaperUnits','Inches','PaperSize',[pos(3), pos(4)])
print(h, [name, '.pdf'],'-dpdf','-r0')
%%%%%%%%%%

name = 'M3130_kb_dirty';
stringname = [path, name];
data = fitsread([stringname, '.fits']);

h = figure;
set(gca, 'LooseInset', get(gca, 'TightInset'));
imagesc(flipud(data(box(1):box(3),box(2):box(4))));
caxis([-0.05, 0.2])
set(h,'Units','Inches');
pos = get(h,'Position');

colormap(col);
c = colorbar;


set(gca,'DataAspectRatio',[1 1 1])
set(gca,'visible','off')
set(h,'PaperPositionMode','Auto','PaperUnits','Inches','PaperSize',[pos(3), pos(4)])
print(h, [name, '.pdf'],'-dpdf','-r0')
%%%%%%%%%%

name = 'M3130_box_residual';
stringname = [path, name];
data = fitsread([stringname, '.fits']);

h = figure;
set(gca, 'LooseInset', get(gca, 'TightInset'));
imagesc(flipud(data(box(1):box(3),box(2):box(4))));
caxis([-0.01,0.01])
set(h,'Units','Inches');
pos = get(h,'Position');

colormap(col);
c = colorbar;


set(gca,'DataAspectRatio',[1 1 1])
set(gca,'visible','off')
set(h,'PaperPositionMode','Auto','PaperUnits','Inches','PaperSize',[pos(3), pos(4)])
print(h, [name, '.pdf'],'-dpdf','-r0')
%%%%%%%%%%

name = 'M3130_kb_residual';
stringname = [path, name];
data = fitsread([stringname, '.fits']);

h = figure;
set(gca, 'LooseInset', get(gca, 'TightInset'));
imagesc(flipud(data(box(1):box(3),box(2):box(4))));
caxis([-0.01,0.01])
set(h,'Units','Inches');
pos = get(h,'Position');

colormap(col);
c = colorbar;


set(gca,'DataAspectRatio',[1 1 1])
set(gca,'visible','off')
set(h,'PaperPositionMode','Auto','PaperUnits','Inches','PaperSize',[pos(3), pos(4)])
print(h, [name, '.pdf'],'-dpdf','-r0')

%%%%%%%%%%

name1 = 'M31_input';
stringname = [path, name1];
data1 = fitsread([stringname, '.fits']);

name2 = 'M3130_kb_solution';
stringname = [path, name2];
data2 = fitsread([stringname, '.fits']);

name = 'M3130_kb_error';
data = data1 - data2;

h = figure;
set(gca, 'LooseInset', get(gca, 'TightInset'));
imagesc(flipud(data(box(1):box(3),box(2):box(4))));
caxis([-0.1,0.1])
set(h,'Units','Inches');
pos = get(h,'Position');

colormap(col);
c = colorbar;


set(gca,'DataAspectRatio',[1 1 1])
set(gca,'visible','off')
set(h,'PaperPositionMode','Auto','PaperUnits','Inches','PaperSize',[pos(3), pos(4)])
print(h, [name, '.pdf'],'-dpdf','-r0')
%%%%%%%%%%

%%%%%%%%%%

name1 = 'M31_input';
stringname = [path, name1];
data1 = fitsread([stringname, '.fits']);

name2 = 'M3130_box_solution';
stringname = [path, name2];
data2 = fitsread([stringname, '.fits']);

name = 'M3130_box_error';
data = data1 - data2;

h = figure;
set(gca, 'LooseInset', get(gca, 'TightInset'));
imagesc(flipud(data(box(1):box(3),box(2):box(4))));

set(h,'Units','Inches');
pos = get(h,'Position');
caxis([-0.1,0.1])
colormap(col);
c = colorbar;


set(gca,'DataAspectRatio',[1 1 1])
set(gca,'visible','off')
set(h,'PaperPositionMode','Auto','PaperUnits','Inches','PaperSize',[pos(3), pos(4)])
print(h, [name, '.pdf'],'-dpdf','-r0')
%%%%%%%%%%