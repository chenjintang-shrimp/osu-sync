import conventionalChangelogAngular from 'conventional-changelog-angular';

export default {
  writerOpts: {
    transform: (commit, context) => {
      const issues = [];

      // 创建一个新的对象而不是修改原对象
      const processedCommit = {
        ...commit,
        type: commit.type || '',
        scope: commit.scope || ''
      };

      // 根据类型设置标题
      if (processedCommit.type === 'feat') {
        processedCommit.type = '🚀 Features';
      } else if (processedCommit.type === 'fix') {
        processedCommit.type = '🐛 Bug Fixes';
      } else if (processedCommit.type === 'refactor' || processedCommit.type === 'chore') {
        processedCommit.type = '🔧 Internal Changes';
      } else if (['docs', 'style', 'perf', 'test'].includes(processedCommit.type)) {
        return false;
      }

      // 缩短 commit hash
      if (typeof processedCommit.hash === 'string') {
        processedCommit.hash = processedCommit.hash.substring(0, 7);
      }

      // 处理破坏性变更
      if (processedCommit.notes && processedCommit.notes.length > 0) {
        processedCommit.notes = processedCommit.notes.map(note => ({
          ...note,
          title: '💥 BREAKING CHANGES'
        }));
      }

      // 处理引用的 Issues
      if (processedCommit.references) {
        processedCommit.references.forEach(reference => {
          issues.push(reference.issue);
        });
      }

      return processedCommit;
    }
  }
};
