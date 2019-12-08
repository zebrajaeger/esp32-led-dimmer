const { src, dest, series, parallel, watch } = require('gulp');
const htmlmin = require('gulp-htmlmin');
const gzip = require('gulp-gzip');
const del = require('del');
var sass = require('gulp-sass');
sass.compiler = require('node-sass');
const cleanCSS = require('gulp-clean-css');
const sourcemaps = require('gulp-sourcemaps');
const uglify = require('gulp-uglify');
const pipeline = require('readable-stream').pipeline;
const rename = require('gulp-rename');
const gulpIF = require('gulp-if');
const browserSync = require('browser-sync');
const server = browserSync.create();

function jsTask() {
    return pipeline(
        src('src/*.js'),
        dest('./dist'),
        sourcemaps.init(),
        uglify(),
        gzip({ gzipOptions: { level: 9 } }),
        sourcemaps.write('.'),
        dest('./dist'));
}

function cssTask() {
    return pipeline(
        src('src/*.scss'),
        sourcemaps.init(),
        sass().on('error', sass.logError),
        dest('./dist'),
        cleanCSS({ compatibility: 'ie8' }),
        gzip({ gzipOptions: { level: 9 } }),
        sourcemaps.write('.'),
        dest('./dist'));
};

function htmlTask() {
    return pipeline(
        src('src/*.html'),
        dest('./dist'),
        htmlmin({ collapseWhitespace: true }),
        gzip({ gzipOptions: { level: 9 } }),
        dest('./dist')
    );
};

function vendorJsTaskCompressed() {
    return pipeline(
        src('vendor/min/*.min.js'),
        gulpIF(function (file) { return file.path.match('jquery-(.*).slim.min.js') }, rename('jquery.slim.min.js')),
        gulpIF(function (file) { return file.path.match('jquery-(.*).min.js') }, rename('jquery.min.js')),
        dest('./dist'),
        gzip({ gzipOptions: { level: 9 } }),
        dest('./dist')
    );
}
function vendorJsTaskUnCompressed() {
    return pipeline(
        src('vendor/*.js'),
        gulpIF(function (file) { return file.path.match('jquery-(.*).slim.js') }, rename('jquery.slim.js')),
        gulpIF(function (file) { return file.path.match('jquery-(.*).js') }, rename('jquery.js')),
        dest('./dist')
    );
}

function copyTask() {
    return pipeline(
        src('dist/*.html.gz', 'dist/*.css.gz', 'dist/*.js.gz'),
        dest('../src/webapp'));
};

function cleanTask() {
    return del(['dist']);
}

function reloadTask(cb) {
    server.reload();
    cb();
}

function watchTask() {
    server.init({
        server: {
          baseDir: 'dist',
        }
      });    watch('src/*.html', series(htmlTask, copyTask, reloadTask));
    watch('src/*.js', series(jsTask, copyTask, reloadTask));
    watch('src/*.scss', series(cssTask, copyTask, reloadTask));
}

exports.clean = cleanTask;
exports.html = htmlTask;
exports.css = cssTask;
exports.js = jsTask;
exports.vendorJs = parallel(vendorJsTaskCompressed, vendorJsTaskUnCompressed);
exports.build = series(cleanTask, parallel(jsTask, htmlTask, cssTask, vendorJsTaskCompressed, vendorJsTaskUnCompressed), copyTask);

exports.default = exports.build;
exports.develop = series(exports.build, watchTask);