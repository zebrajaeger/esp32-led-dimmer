// !!!!! AUTO GENERATED, DO NOT EDIT !!!!!

#pragma once

webServer_.on("/favicon.ico", [this]() {
  webServer_.sendHeader("Content-Encoding", "gzip");
  webServer_.send_P(200, "image/vnd.microsoft.icon", favicon_ico_gz_start, favicon_ico_gz_end - favicon_ico_gz_start - 1);
});

webServer_.on("/index.html", [this]() {
  webServer_.sendHeader("Content-Encoding", "gzip");
  webServer_.send_P(200, "text/html", index_html_gz_start, index_html_gz_end - index_html_gz_start - 1);
});

webServer_.on("/main.js", [this]() {
  webServer_.sendHeader("Content-Encoding", "gzip");
  webServer_.send_P(200, "application/javascript", main_js_gz_start, main_js_gz_end - main_js_gz_start - 1);
});

webServer_.on("/ngsw-worker.js", [this]() {
  webServer_.sendHeader("Content-Encoding", "gzip");
  webServer_.send_P(200, "application/javascript", ngsw_worker_js_gz_start, ngsw_worker_js_gz_end - ngsw_worker_js_gz_start - 1);
});

webServer_.on("/polyfills.js", [this]() {
  webServer_.sendHeader("Content-Encoding", "gzip");
  webServer_.send_P(200, "application/javascript", polyfills_js_gz_start, polyfills_js_gz_end - polyfills_js_gz_start - 1);
});

webServer_.on("/manifest.webmanifest", [this]() {
  webServer_.sendHeader("Content-Encoding", "gzip");
  webServer_.send_P(200, "application/manifest+json", manifest_webmanifest_gz_start, manifest_webmanifest_gz_end - manifest_webmanifest_gz_start - 1);
});

webServer_.on("/ngsw.json", [this]() {
  webServer_.sendHeader("Content-Encoding", "gzip");
  webServer_.send_P(200, "application/json", ngsw_json_gz_start, ngsw_json_gz_end - ngsw_json_gz_start - 1);
});

webServer_.on("/safety-worker.js", [this]() {
  webServer_.sendHeader("Content-Encoding", "gzip");
  webServer_.send_P(200, "application/javascript", safety_worker_js_gz_start, safety_worker_js_gz_end - safety_worker_js_gz_start - 1);
});

webServer_.on("/runtime.js", [this]() {
  webServer_.sendHeader("Content-Encoding", "gzip");
  webServer_.send_P(200, "application/javascript", runtime_js_gz_start, runtime_js_gz_end - runtime_js_gz_start - 1);
});

webServer_.on("/worker-basic.min.js", [this]() {
  webServer_.sendHeader("Content-Encoding", "gzip");
  webServer_.send_P(200, "application/javascript", worker_basic_min_js_gz_start, worker_basic_min_js_gz_end - worker_basic_min_js_gz_start - 1);
});

webServer_.on("/styles.css", [this]() {
  webServer_.sendHeader("Content-Encoding", "gzip");
  webServer_.send_P(200, "text/css", styles_css_gz_start, styles_css_gz_end - styles_css_gz_start - 1);
});

webServer_.on("/icon-128x128.png", [this]() {
  webServer_.sendHeader("Content-Encoding", "gzip");
  webServer_.send_P(200, "image/png", assets_icons_icon_128x128_png_gz_start, assets_icons_icon_128x128_png_gz_end - assets_icons_icon_128x128_png_gz_start - 1);
});

webServer_.on("/icon-192x192.png", [this]() {
  webServer_.sendHeader("Content-Encoding", "gzip");
  webServer_.send_P(200, "image/png", assets_icons_icon_192x192_png_gz_start, assets_icons_icon_192x192_png_gz_end - assets_icons_icon_192x192_png_gz_start - 1);
});

webServer_.on("/icon-144x144.png", [this]() {
  webServer_.sendHeader("Content-Encoding", "gzip");
  webServer_.send_P(200, "image/png", assets_icons_icon_144x144_png_gz_start, assets_icons_icon_144x144_png_gz_end - assets_icons_icon_144x144_png_gz_start - 1);
});

webServer_.on("/icon-152x152.png", [this]() {
  webServer_.sendHeader("Content-Encoding", "gzip");
  webServer_.send_P(200, "image/png", assets_icons_icon_152x152_png_gz_start, assets_icons_icon_152x152_png_gz_end - assets_icons_icon_152x152_png_gz_start - 1);
});

webServer_.on("/icon-384x384.png", [this]() {
  webServer_.sendHeader("Content-Encoding", "gzip");
  webServer_.send_P(200, "image/png", assets_icons_icon_384x384_png_gz_start, assets_icons_icon_384x384_png_gz_end - assets_icons_icon_384x384_png_gz_start - 1);
});

webServer_.on("/icon-512x512.png", [this]() {
  webServer_.sendHeader("Content-Encoding", "gzip");
  webServer_.send_P(200, "image/png", assets_icons_icon_512x512_png_gz_start, assets_icons_icon_512x512_png_gz_end - assets_icons_icon_512x512_png_gz_start - 1);
});

webServer_.on("/icon-72x72.png", [this]() {
  webServer_.sendHeader("Content-Encoding", "gzip");
  webServer_.send_P(200, "image/png", assets_icons_icon_72x72_png_gz_start, assets_icons_icon_72x72_png_gz_end - assets_icons_icon_72x72_png_gz_start - 1);
});

webServer_.on("/icon-96x96.png", [this]() {
  webServer_.sendHeader("Content-Encoding", "gzip");
  webServer_.send_P(200, "image/png", assets_icons_icon_96x96_png_gz_start, assets_icons_icon_96x96_png_gz_end - assets_icons_icon_96x96_png_gz_start - 1);
});

