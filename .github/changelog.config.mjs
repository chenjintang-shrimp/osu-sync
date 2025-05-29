import conventionalChangelogAngular from 'conventional-changelog-angular';

export default {
  writerOpts: {
    transform: (commit, context) => {
      const issues = [];

      commit.type = commit.type || '';
      commit.scope = commit.scope || '';

      if (commit.type === 'feat') {
        commit.type = '🚀 Features';
      } else if (commit.type === 'fix') {
        commit.type = '🐛 Bug Fixes';
      } else if (commit.type === 'refactor' || commit.type === 'chore') {
        commit.type = '🔧 Internal Changes';
      } else if (['docs', 'style', 'perf', 'test'].includes(commit.type)) {
        return false;
      }

      if (typeof commit.hash === 'string') {
        commit.hash = commit.hash.substring(0, 7);
      }

      // Breaking Changes
      if (commit.notes.length > 0) {
        commit.notes.forEach(note => {
          note.title = '💥 BREAKING CHANGES';
        });
        return commit;
      }

      // Issues
      if (commit.references) {
        commit.references.forEach(reference => {
          issues.push(reference.issue);
        });
      }

      return commit;
    }
  }
};
