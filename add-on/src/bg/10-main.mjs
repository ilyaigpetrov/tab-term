(async () => {
  await globalThis.migrationPromise;
  console.log('Migration is over. Main bg/worker starts.');

  chrome.browserAction.onClicked.addListener(() => {
    chrome.tabs.create({
      url: './src/pages/terminal/index.html',
    });
  });

})();
