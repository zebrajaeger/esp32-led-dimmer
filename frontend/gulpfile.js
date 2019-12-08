const {src, dest, series} = require('gulp');
const gzip = require('gulp-gzip');
const del = require('del');
const readDir = require('recursive-readdir');
const path = require('path');
const fs = require('fs');
const mimeTypes = require('mime-types');


const srcDir = './dist/Esp32LedDimmer/';
const targetDir = './../src/webapp/';
const symbolFilePath = targetDir + "files.h";
const webserverFilePath = targetDir + "webserver.h";

function cleanTargetTask() {
  return del([targetDir + '*', targetDir + 'assets'], {force: true});
}

function copyTask() {
  return src(srcDir + '**/*')
    .pipe(gzip({gzipOptions: {level: 9}}))
    .pipe(dest(targetDir));
}

function fileListTask(cb) {
  readDir(srcDir, ['*.txt'], (err, files) => {
    let fileHContent = '// !!!!! AUTO GENERATED, DO NOT EDIT !!!!!\n\n' + '#pragma once\n\n';
    let webserverHContent = '// !!!!! AUTO GENERATED, DO NOT EDIT !!!!!\n\n' + '#pragma once\n\n';
    for (const f of files) {
      const ext = path.extname(f).toLowerCase();
      const name = path.posix.basename(f);
      const relativePath = path.relative(srcDir, f) + ".gz";
      const symbol = relativePath.replace(/\.|\/|\\|-/g, '_');
      const startSymbol = symbol + '_start';
      const endSymbol = symbol + '_end';
      const mime = mimeTypes.lookup(ext);

      fileHContent += '// ' + relativePath + '\n';
      fileHContent += 'extern const char ' + startSymbol +'[] asm("_binary_src_webapp_' + startSymbol + '");\n';
      fileHContent += 'extern const char ' + endSymbol +'[] asm("_binary_src_webapp_' + endSymbol + '");\n\n';

      webserverHContent += 'webServer_.on("/' + name + '", [this]() {\n';
      webserverHContent += '  webServer_.sendHeader("Content-Encoding", "gzip");\n';
      webserverHContent += '  webServer_.send_P(200, "' + mime + '", ' + startSymbol + ', ' + endSymbol + ' - ' + startSymbol + ' - 1);\n';

      webserverHContent += '});\n\n';
    }
    fs.writeFileSync(symbolFilePath, fileHContent);
    fs.writeFileSync(webserverFilePath, webserverHContent);
    cb();
  });
}

// exports.default = series(cleanTargetTask, copyTask);
exports.clean = cleanTargetTask;
exports.copy = copyTask;
exports.default = fileListTask;
